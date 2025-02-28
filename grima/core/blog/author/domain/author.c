#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <shared/result.h>
#include <shared/strings.h>

#include <hashmap.h>

#include "../author.h"

int compare_author_validation_error(const void *a, const void *b, void *udata) {
  const struct author_validation_error *ua = a;
  const struct author_validation_error *ub = b;

  return strcmp(ua->field, ub->field);
}

uint64_t hash_author_validation_error(const void *item, uint64_t seed0, uint64_t seed1) {
  const struct author_validation_error *entry = item;
  return hashmap_sip(entry->field, strlen(entry->field), seed0, seed1);
}

char **validate_username(char *username) {
  if (username == NULL) {
    char **errors = malloc(2 * sizeof(char *));
    errors[0] = "is required";
    errors[1] = NULL;

    return errors;
  }

  if (strcmp(username, "") == 0) {
    char **errors = malloc(2 * sizeof(char *));
    errors[0] = "can't be blank";
    errors[1] = NULL;

    return errors;
  }

  size_t error_count = 0;
  char **errors = NULL;

  for (size_t i = 0; i < strlen(username); i++) {
    if (!isalnum(username[i]) && username[i] != '_') {
      errors = realloc(errors, sizeof(char *) * (error_count + 1));
      char *message = "must only contain letters, numbers or underscores";
      errors[error_count] = malloc(sizeof(char) * (strlen(message) + 1));
      strcpy(errors[error_count], message);
      error_count++;

      break;
    };
  }

  size_t username_length = strlen(username);

  if (username_length < 4) {
    errors = realloc(errors, sizeof(char *) * (error_count + 1));
    char *message = "must be at least 4 characters";
    errors[error_count] = malloc(sizeof(char) * (strlen(message) + 1));
    strcpy(errors[error_count], message);
    error_count++;
  }

  if (username_length > 32) {
    errors = realloc(errors, sizeof(char *) * (error_count + 1));
    char *message = "must be at most 32 characters";
    errors[error_count] = malloc(sizeof(char) * (strlen(message) + 1));
    strcpy(errors[error_count], message);
    error_count++;
  }

  if (error_count == 0)
    return NULL;

  errors = realloc(errors, sizeof(char *) * (error_count + 1));
  errors[error_count] = NULL;

  return errors;
}

author_validation_errors validate_author(struct author *author) {
  author_validation_errors validation_errors =
      hashmap_new(sizeof(struct author_validation_error), 1, 0, 0, hash_author_validation_error,
                  compare_author_validation_error, NULL, NULL);

  char **username_validation_errors = validate_username(author->username);

  if (username_validation_errors != NULL) {
    struct author_validation_error username_error = {
        .field = "username",
        .errors = username_validation_errors,
    };

    hashmap_set(validation_errors, &username_error);
  }

  if (hashmap_count(validation_errors) == 0) {
    hashmap_free(validation_errors);
    return NULL;
  }

  return validation_errors;
}

struct create_author_result Author_create(char *username, char *bio, char *image) {
  struct create_author_result result;

  char *clean_username = trim_whitespace(username);
  char *clean_bio = is_blank_string(bio) ? NULL : trim_whitespace(bio);
  char *clean_image = is_blank_string(image) ? NULL : trim_whitespace(image);

  struct author *author = malloc(sizeof(struct author));

  author->username = clean_username;
  author->bio = clean_bio;
  author->image = clean_image;

  author_validation_errors validation_errors = validate_author(author);

  if (validation_errors != NULL) {
    free(author);

    result.success = false;

    result.value.error = (struct author_creation_error){
        .kind = AUTHOR_VALIDATION_ERROR,
        .error =
            {
                .validation_errors = validation_errors,
            },
    };
    ;

    return result;
  }

  epoch_ms_t now = current_unix_timestamp();
  author->created_at = now;
  author->updated_at = now;
  strcpy(author->id, AuthorRepository_get_next_id());

  result.success = true;
  result.value.success = author;

  return result;
}

void Author_free(struct author *author) {
  if (author->id != NULL)
    free(author->id);

  if (author->bio != NULL)
    free(author->bio);

  if (author->image != NULL)
    free(author->image);

  if (author->id != NULL)
    free(author->id);

  free(author);
}

#ifdef DEV
#include <stdio.h>

#include <uuid/uuid.h>

void Author_show(struct author *author) {
  char created_at_str[ISO_TIME_LENGTH];
  unix_timestamp_to_iso8601(author->created_at, created_at_str);

  char updated_at_str[ISO_TIME_LENGTH];
  unix_timestamp_to_iso8601(author->updated_at, updated_at_str);

  printf("Author {\n\tID = \"%s\","
         "\n\tusername = \"%s\",\n\tbio = \"%s\",\n\timage = \"%s\",\n\tcreated_at = "
         "\"%s\"\n\tupdated_at = \"%s\"\n}\n",
         author->id, author->username, author->bio, author->image, created_at_str, updated_at_str);
}

void Author_show_creation_error(struct author_creation_error creation_error) {
  if (creation_error.kind == AUTHOR_VALIDATION_ERROR) {
    author_validation_errors validation_errors = creation_error.error.validation_errors;

    void *item;
    size_t i = 0;

    while (hashmap_iter(validation_errors, &i, &item)) {
      struct author_validation_error *validation_error = item;

      printf("[.%s]\n", validation_error->field);

      char **errors = validation_error->errors;

      for (size_t j = 0; errors[j]; j++) {
        printf("  - %s\n", errors[j]);
      }
    }
  } else {
    printf("Error: %d\n", creation_error.kind);
  }
}

#endif
