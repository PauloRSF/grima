#include <stdlib.h>
#include <string.h>

#include <libpq-fe.h>

#include <lib/database.h>
#include <lib/id.h>
#include <shared/date.h>

#include "article.h"

entity_id_t ArticleRepository_get_next_id() { return generate_entity_id(); };

struct pg_query_params serialize_article_to_pg_params(struct article *article) {
  struct pg_query_params params = new_pg_query_params();

  add_pg_query_param(&params, article->id);
  add_pg_query_param(&params, article->author_id);
  add_pg_query_param(&params, article->slug);
  add_pg_query_param(&params, article->title);
  add_pg_query_param(&params, article->description);
  add_pg_query_param(&params, article->body);

  char *created_at = unix_timestamp_to_iso8601(article->created_at);
  add_pg_query_param(&params, created_at);
  free(created_at);

  char *updated_at = unix_timestamp_to_iso8601(article->updated_at);
  add_pg_query_param(&params, updated_at);
  free(updated_at);

  return params;
}

enum article_repository_save_error map_query_error_to_article_save_error(PGresult *result) {
  if (has_constraint_violation(result, "article_unique_slug"))
    return ARTICLE_REPOSITORY_SAVE_SLUG_ALREADY_TAKEN;

  if (has_constraint_violation(result, "article_belongs_to_author"))
    return ARTICLE_REPOSITORY_SAVE_AUTHOR_NOT_FOUND;

  return ARTICLE_REPOSITORY_SAVE_APPLICATION_ERROR;
}

struct article_repository_save_result ArticleRepository_save(struct article *article) {
  char *query = "INSERT INTO articles (id, author_id, slug, title, description, body, created_at, updated_at) VALUES "
                "($1, $2, $3, $4, $5, $6, $7, $8)";

  struct pg_query_params params = serialize_article_to_pg_params(article);

  PGresult *query_result = pg_query(query, params);

  free_pg_query_params(params);

  struct article_repository_save_result save_result = {
      .success = PQresultStatus(query_result) == PGRES_COMMAND_OK,
  };

  if (!save_result.success) {
    save_result.value.error = map_query_error_to_article_save_error(query_result);
  }

  PQclear(query_result);

  return save_result;
}

struct article_repository_exists_by_id_result ArticleRepository_exists_by_id(entity_id_t article_id) {
  struct pg_query_params params = new_pg_query_params();

  add_pg_query_param(&params, article_id);

  PGresult *query_result = pg_query("SELECT COUNT(*) FROM articles WHERE id = $1", params);

  free_pg_query_params(params);

  struct article_repository_exists_by_id_result exists_result = {
      .success = PQresultStatus(query_result) == PGRES_TUPLES_OK,
  };

  if (exists_result.success) {
    char *count_string = PQgetvalue(query_result, 0, 0);

    exists_result.value.success = atoi(count_string) > 0;
  }

  PQclear(query_result);

  return exists_result;
}
