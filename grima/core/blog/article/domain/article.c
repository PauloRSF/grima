#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <sys/random.h>
#include <uuid/uuid.h>

#include <shared/errors.h>
#include <shared/result.h>
#include <shared/strings.h>

#include "../article.h"

char *validate_article_author_id(char *author_id) {
  if (author_id == NULL)
    return "is required";

  uuid_t uuid;

  if (uuid_parse(author_id, uuid) != 0)
    return "must be a valid UUID";

  return NULL;
}

char *validate_article_title(char *title) {
  if (title == NULL)
    return "is required";
  if (strcmp(title, "") == 0)
    return "can't be blank";

  size_t title_length = strlen(title);

  if (title_length < 4)
    return "must be at least 4 characters";
  if (title_length > 127)
    return "must be at most 127 characters";

  return NULL;
}

char *validate_article_description(char *description) {
  if (description == NULL)
    return "is required";
  if (strcmp(description, "") == 0)
    return "can't be blank";
  if (strlen(description) > 255)
    return "must be at most 255 characters";

  return NULL;
}

char *validate_article_body(char *body) {
  if (body == NULL)
    return "is required";
  if (strcmp(body, "") == 0)
    return "can't be blank";

  return NULL;
}

ValidationErrors validate_article(struct article *article) {
  ValidationErrors validation_errors = ValidationErrors_new();

  char *author_id_validation_error = validate_article_author_id(article->author_id);

  if (author_id_validation_error != NULL) {
    ValidationErrors_add(validation_errors, "author_id", author_id_validation_error);
  }

  char *title_validation_error = validate_article_title(article->title);

  if (title_validation_error != NULL) {
    ValidationErrors_add(validation_errors, "title", title_validation_error);
  }

  char *description_validation_error = validate_article_description(article->description);

  if (description_validation_error != NULL) {
    ValidationErrors_add(validation_errors, "description", description_validation_error);
  }

  char *body_validation_error = validate_article_body(article->body);

  if (body_validation_error != NULL) {
    ValidationErrors_add(validation_errors, "body", body_validation_error);
  }

  if (ValidationErrors_fields_count(validation_errors) == 0) {
    return NULL;
  }

  return validation_errors;
}

char *Article_slugify_title(char *title) {
  size_t title_length = strlen(title);
  size_t slug_length = title_length + 7;
  char *slug = malloc(sizeof(char) * (slug_length + 1));
  size_t slug_index = 0;

  for (size_t i = 0; i < title_length; i++) {
    if (title[i] == ' ') {
      slug[slug_index++] = '-';
    } else if (isalnum(title[i])) {
      slug[slug_index++] = tolower(title[i]);
    }
  }

  char bytes[6];
  getrandom(bytes, 6, 0);

  slug[slug_index++] = '-';

  for (size_t i = 0; i < 6; i++) {
    slug[slug_index++] = '0' + (bytes[i] % 10);
  }

  slug[slug_index] = '\0';

  return realloc(slug, sizeof(char) * slug_index);
}

struct create_article_result Article_create(char *author_id, char *title, char *description, char *body) {
  struct create_article_result result;

  char *trimmed_title = clone_and_trim_string(title);
  char *trimmed_description = clone_and_trim_string(description);
  char *trimmed_body = clone_and_trim_string(body);

  struct article *article = malloc(sizeof(struct article));

  article->author_id = author_id;
  article->title = trimmed_title;
  article->description = trimmed_description;
  article->body = trimmed_body;

  ValidationErrors validation_errors = validate_article(article);

  if (validation_errors != NULL) {
    free(article);

    result.success = false;
    result.value.error = validation_errors;

    return result;
  }

  epoch_ms_t now = current_unix_timestamp();
  article->created_at = now;
  article->updated_at = now;
  article->slug = Article_slugify_title(article->title);
  article->id = ArticleRepository_get_next_id();

  result.success = true;
  result.value.success = article;

  return result;
}

void Article_free(struct article *article) {
  if (article->id != NULL)
    free(article->id);

  if (article->title != NULL)
    free(article->title);

  if (article->description != NULL)
    free(article->description);

  if (article->body != NULL)
    free(article->body);

  free(article);
}

#ifdef DEV
#include <stdio.h>

#include <uuid/uuid.h>

void Article_show(struct article *article) {
  printf("Article {\n\tid = \"%s\",\n\tauthor_id = \"%s\",\n\tslug = \"%s\","
         "\n\ttitle = \"%s\",\n\tdescription = \"%s\",\n\tbody = \"%s\",\n\tcreated_at = "
         "\"%s\"\n\tupdated_at = \"%s\"\n}\n",
         article->id, article->author_id, article->slug, article->title, article->description, article->body,
         unix_timestamp_to_iso8601(article->created_at), unix_timestamp_to_iso8601(article->updated_at));
}

void Article_show_creation_result(struct create_article_result result) {
  printf("Article creation: %s\n", result.success ? "success" : "failed");

  if (result.success) {
    Article_show(result.value.success);
  } else {
    ValidationErrors_show(result.value.error);
  }
}

#endif
