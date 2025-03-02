#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <shared/errors.h>
#include <shared/result.h>
#include <shared/strings.h>

#include "../author.h"

char **validate_author_username(char *username) {
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

ValidationErrors validate_author(struct author *author) {
  ValidationErrors validation_errors = ValidationErrors_new();

  char **username_validation_errors = validate_author_username(author->username);

  if (username_validation_errors != NULL) {
    size_t username_validation_errors_count = 0;

    while (username_validation_errors[username_validation_errors_count] != NULL)
      username_validation_errors_count++;

    ValidationErrors_add_many(validation_errors, "username", username_validation_errors,
                              username_validation_errors_count);
  }

  if (ValidationErrors_fields_count(validation_errors) == 0) {
    return NULL;
  }

  return validation_errors;
}

struct create_author_result Author_create(char *username, char *bio, char *image) {
  struct create_author_result result;

  char *trimmed_username = clone_and_trim_string(username);
  char *trimmed_bio = is_blank_string(bio) ? NULL : clone_and_trim_string(bio);
  char *trimmed_image = is_blank_string(image) ? NULL : clone_and_trim_string(image);

  struct author *author = malloc(sizeof(struct author));

  author->username = trimmed_username;
  author->bio = trimmed_bio;
  author->image = trimmed_image;

  ValidationErrors validation_errors = validate_author(author);

  if (validation_errors != NULL) {
    free(author);

    result.success = false;
    result.value.error = validation_errors;

    return result;
  }

  epoch_ms_t now = current_unix_timestamp();
  author->created_at = now;
  author->updated_at = now;
  author->id = AuthorRepository_get_next_id();

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
  printf("Author {\n\tid = \"%s\","
         "\n\tusername = \"%s\",\n\tbio = \"%s\",\n\timage = \"%s\",\n\tcreated_at = "
         "\"%s\"\n\tupdated_at = \"%s\"\n}\n",
         author->id, author->username, author->bio, author->image, unix_timestamp_to_iso8601(author->created_at), unix_timestamp_to_iso8601(author->updated_at));
}

void Author_show_creation_result(struct create_author_result result) {
  printf("Author creation: %s\n", result.success ? "success" : "failed");

  if (result.success) {
    Author_show(result.value.success);
  } else {
    ValidationErrors_show(result.value.error);
  }
}

#endif
