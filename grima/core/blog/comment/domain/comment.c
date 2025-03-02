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

#include "../comment.h"

char *validate_comment_relation_id(char *author_id) {
  if (author_id == NULL)
    return "is required";

  uuid_t uuid;

  if (uuid_parse(author_id, uuid) != 0)
    return "must be a valid UUID";

  return NULL;
}

char *validate_comment_body(char *body) {
  if (body == NULL)
    return "is required";
  if (strcmp(body, "") == 0)
    return "can't be blank";

  return NULL;
}

ValidationErrors validate_comment(struct comment *comment) {
  ValidationErrors validation_errors = ValidationErrors_new();

  char *author_id_validation_error = validate_comment_relation_id(comment->author_id);

  if (author_id_validation_error != NULL) {
    ValidationErrors_add(validation_errors, "author_id", author_id_validation_error);
  }

  char *article_id_validation_error = validate_comment_relation_id(comment->article_id);

  if (article_id_validation_error != NULL) {
    ValidationErrors_add(validation_errors, "article_id", article_id_validation_error);
  }

  char *body_validation_error = validate_comment_body(comment->body);

  if (body_validation_error != NULL) {
    ValidationErrors_add(validation_errors, "body", body_validation_error);
  }

  if (ValidationErrors_fields_count(validation_errors) == 0) {
    return NULL;
  }

  return validation_errors;
}

struct create_comment_result Comment_create(char *author_id, char *article_id, char *body) {
  struct create_comment_result result;

  char *trimmed_body = clone_and_trim_string(body);

  struct comment *comment = malloc(sizeof(struct comment));

  comment->author_id = author_id;
  comment->article_id = article_id;
  comment->body = trimmed_body;

  ValidationErrors validation_errors = validate_comment(comment);

  if (validation_errors != NULL) {
    free(comment);

    result.success = false;
    result.value.error = validation_errors;

    return result;
  }

  epoch_ms_t now = current_unix_timestamp();
  comment->created_at = now;
  comment->updated_at = now;
  comment->id = CommentRepository_get_next_id();

  result.success = true;
  result.value.success = comment;

  return result;
}

void Comment_free(struct comment *comment) {
  if (comment->id != NULL)
    free(comment->id);

  if (comment->author_id != NULL)
    free(comment->author_id);

  if (comment->article_id != NULL)
    free(comment->article_id);

  if (comment->body != NULL)
    free(comment->body);

  free(comment);
}

#ifdef DEV
#include <stdio.h>

#include <uuid/uuid.h>

void Comment_show(struct comment *comment) {
  printf("Comment {\n\tid = \"%s\",\n\tauthor_id = \"%s\",\n\article_id = \"%s\",\n\tbody = \"%s\",\n\tcreated_at = "
         "\"%s\"\n\tupdated_at = \"%s\"\n}\n",
         comment->id, comment->author_id, comment->article_id, comment->body, unix_timestamp_to_iso8601(comment->created_at), unix_timestamp_to_iso8601(comment->updated_at));
}

void Comment_show_creation_result(struct create_comment_result result) {
  printf("Comment creation result: %s\n", result.success ? "success" : "failure");

  if (result.success) {
    Comment_show(result.value.success);
  } else {
    ValidationErrors_show(result.value.error);
  }
}

#endif
