#include <stdlib.h>

#include <libpq-fe.h>

#include <lib/database.h>
#include <lib/id.h>
#include <shared/date.h>

#include "comment.h"

entity_id_t CommentRepository_get_next_id() { return generate_entity_id(); };

struct pg_query_params serialize_comment_to_pg_params(struct comment *comment) {
  struct pg_query_params params = new_pg_query_params();

  add_pg_query_param(&params, comment->id);
  add_pg_query_param(&params, comment->author_id);
  add_pg_query_param(&params, comment->article_id);
  add_pg_query_param(&params, comment->body);

  char *created_at = unix_timestamp_to_iso8601(comment->created_at);
  add_pg_query_param(&params, created_at);
  free(created_at);

  char *updated_at = unix_timestamp_to_iso8601(comment->updated_at);
  add_pg_query_param(&params, updated_at);
  free(updated_at);

  return params;
}

enum comment_repository_save_error map_query_error_to_comment_save_error(PGresult *result) {
  if (has_constraint_violation(result, "comment_belongs_to_author"))
    return COMMENT_REPOSITORY_SAVE_AUTHOR_NOT_FOUND;

  if (has_constraint_violation(result, "comment_belongs_to_article"))
    return COMMENT_REPOSITORY_SAVE_ARTICLE_NOT_FOUND;

  return COMMENT_REPOSITORY_SAVE_APPLICATION_ERROR;
}

struct comment_repository_save_result CommentRepository_save(struct comment *comment) {
  char *query =
      "INSERT INTO comments (id, author_id, article_id, body, created_at, updated_at) VALUES ($1, $2, $3, $4, $5, $6)";

  struct pg_query_params params = serialize_comment_to_pg_params(comment);

  PGresult *query_result = pg_query(query, params);

  free_pg_query_params(params);

  struct comment_repository_save_result save_result = {
      .success = PQresultStatus(query_result) == PGRES_COMMAND_OK,
  };

  if (!save_result.success) {
    save_result.value.error = map_query_error_to_comment_save_error(query_result);
  }

  PQclear(query_result);

  return save_result;
}
