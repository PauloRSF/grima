#include <stdlib.h>
#include <string.h>

#include <libpq-fe.h>

#include <lib/database.h>
#include <lib/id.h>
#include <shared/date.h>

#include "favorite.h"

entity_id_t FavoriteRepository_get_next_id() { return generate_entity_id(); };

struct pg_query_params serialize_favorite_to_pg_params(struct favorite *favorite) {
  struct pg_query_params params = new_pg_query_params();

  add_pg_query_param(&params, favorite->id);
  add_pg_query_param(&params, favorite->author_id);
  add_pg_query_param(&params, favorite->article_id);

  char *created_at = unix_timestamp_to_iso8601(favorite->created_at);
  add_pg_query_param(&params, created_at);
  free(created_at);

  return params;
}

enum favorite_repository_save_error map_favorite_save_postgres_error_to_error_enum(PGresult *result) {
  if (has_constraint_violation(result, "favorite_belongs_to_author"))
    return FAVORITE_REPOSITORY_SAVE_AUTHOR_NOT_FOUND;

  if (has_constraint_violation(result, "favorite_belongs_to_article"))
    return FAVORITE_REPOSITORY_SAVE_ARTICLE_NOT_FOUND;

  return FAVORITE_REPOSITORY_SAVE_APPLICATION_ERROR;
}

struct favorite_repository_save_result FavoriteRepository_save(struct favorite *favorite) {
  struct favorite_repository_exists_result favorite_exists_result =
      FavoriteRepository_exists(favorite->author_id, favorite->article_id);

  if (!favorite_exists_result.success) {
    return (struct favorite_repository_save_result){
        .success = false,
        .value.error = FAVORITE_REPOSITORY_SAVE_APPLICATION_ERROR,
    };
  }

  bool favorite_exists = favorite_exists_result.value.success;

  if (favorite_exists) {
    return (struct favorite_repository_save_result){
        .success = true,
    };
  }

  char *query = "INSERT INTO favorites (id, author_id, article_id, created_at) VALUES ($1, $2, $3, $4)";

  struct pg_query_params params = serialize_favorite_to_pg_params(favorite);

  PGresult *result = pg_query(query, params);

  free_pg_query_params(params);

  struct favorite_repository_save_result save_result = {
      .success = PQresultStatus(result) == PGRES_COMMAND_OK,
  };

  if (!save_result.success) {
    save_result.value.error = map_favorite_save_postgres_error_to_error_enum(result);
  }

  PQclear(result);

  return save_result;
}

struct favorite_repository_exists_result FavoriteRepository_exists(entity_id_t author_id, entity_id_t article_id) {
  struct pg_query_params params = new_pg_query_params();

  add_pg_query_param(&params, author_id);
  add_pg_query_param(&params, article_id);

  PGresult *result = pg_query("SELECT COUNT(*) FROM favorites WHERE author_id = $1 AND article_id = $2", params);

  free_pg_query_params(params);

  struct favorite_repository_exists_result result_value = {
      .success = PQresultStatus(result) == PGRES_TUPLES_OK,
  };

  if (result_value.success) {
    char *count_string = PQgetvalue(result, 0, 0);

    result_value.value.success = atoi(count_string) > 0;
  }

  PQclear(result);

  return result_value;
}
