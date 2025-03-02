#include <stdlib.h>
#include <string.h>

#include <cpino.h>
#include <libpq-fe.h>
#include <uuid/uuid.h>

#include <lib/database.h>
#include <shared/date.h>

#include "../../author/author.h"
#include "../../article/article.h"
#include "../comment.h"

#define COMMENT_PARAMS_COUNT 6

char *CommentRepository_get_next_id() {
  uuid_t comment_id;

  uuid_generate(comment_id);

  char *comment_id_string = malloc(37 * sizeof(char));

  uuid_unparse(comment_id, comment_id_string);

  return comment_id_string;
};

struct comment_params {
  const char *values[COMMENT_PARAMS_COUNT];
  int lengths[COMMENT_PARAMS_COUNT];
  int formats[COMMENT_PARAMS_COUNT];
};

struct comment_params build_comment_params(struct comment *comment) {
  struct comment_params params = {
      .values = {0},
      .lengths = {0},
      .formats = {0},
  };

  params.values[0] = comment->id;
  params.lengths[0] = strlen(comment->id);
  params.formats[0] = 0;

  params.values[1] = comment->author_id;
  params.lengths[1] = strlen(comment->author_id);
  params.formats[1] = 0;

  params.values[2] = comment->article_id;
  params.lengths[2] = strlen(comment->article_id);
  params.formats[2] = 0;

  params.values[3] = comment->body;
  params.lengths[3] = strlen(comment->body);
  params.formats[3] = 0;

  params.values[4] = unix_timestamp_to_iso8601(comment->updated_at);
  params.lengths[4] = ISO_TIME_LENGTH;
  params.formats[4] = 0;

  params.values[5] = unix_timestamp_to_iso8601(comment->updated_at);
  params.lengths[5] = ISO_TIME_LENGTH;
  params.formats[5] = 0;

  return params;
}

enum comment_repository_save_error map_comment_save_postgres_error_to_domain_error(PGresult *result) {
  char *error_type = PQresultErrorField(result, PG_DIAG_SQLSTATE);

  bool is_foreign_key_constraint_violation = strcmp(error_type, POSTGRES_FOREIGN_KEY_VIOLATION_ERROR_CODE) == 0;

  if (!is_foreign_key_constraint_violation)
    return COMMENT_REPOSITORY_SAVE_APPLICATION_ERROR;

  char *constraint_name = PQresultErrorField(result, PG_DIAG_CONSTRAINT_NAME);

  if (strcmp(constraint_name, "comment_has_author") == 0)
    return COMMENT_REPOSITORY_SAVE_AUTHOR_NOT_FOUND;

  if (strcmp(constraint_name, "comment_has_article") == 0)
    return COMMENT_REPOSITORY_SAVE_ARTICLE_NOT_FOUND;

  return COMMENT_REPOSITORY_SAVE_APPLICATION_ERROR;
}

struct comment_repository_save_result CommentRepository_save(struct comment *comment) {
  PGconn *connection = get_database_connection();
  struct comment_repository_save_result save_result;

  struct author_repository_exists_by_id_result author_exists_result = AuthorRepository_exists_by_id(comment->author_id);

  if (!author_exists_result.success) {
    save_result.success = false;
    save_result.value.error = COMMENT_REPOSITORY_SAVE_APPLICATION_ERROR;

    return save_result;
  } else if (author_exists_result.value.success == false) {
    save_result.success = false;
    save_result.value.error = COMMENT_REPOSITORY_SAVE_AUTHOR_NOT_FOUND;

    return save_result;
  }

  struct article_repository_exists_by_id_result article_exists_result = ArticleRepository_exists_by_id(comment->article_id);

  if (!article_exists_result.success) {
    save_result.success = false;
    save_result.value.error = COMMENT_REPOSITORY_SAVE_APPLICATION_ERROR;

    return save_result;
  } else if (article_exists_result.value.success == false) {
    save_result.success = false;
    save_result.value.error = COMMENT_REPOSITORY_SAVE_ARTICLE_NOT_FOUND;

    return save_result;
  }

  const char *query = "INSERT INTO comments (id, author_id, article_id, body, created_at, updated_at) "
                      "VALUES ($1, $2, $3, $4, $5, $6)";

  struct comment_params params = build_comment_params(comment);

  cpino_log_debug("[DATABASE] %s", query);

  PGresult *result =
      PQexecParams(connection, query, COMMENT_PARAMS_COUNT, NULL, params.values, params.lengths, params.formats, 0);

  if (PQresultStatus(result) == PGRES_COMMAND_OK) {
    PQclear(result);

    save_result.success = true;

    return save_result;
  }

  char *message = PQresultVerboseErrorMessage(result, PQERRORS_VERBOSE, PQSHOW_CONTEXT_ALWAYS);

  cpino_log_error("[DATABASE] Failed to insert Comment: %s", message);

  PQfreemem(message);

  save_result.success = false;
  save_result.value.error = map_comment_save_postgres_error_to_domain_error(result);

  PQclear(result);

  return save_result;
}
