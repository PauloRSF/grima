#include <stdlib.h>
#include <string.h>

#include <lib/database.h>
#include <lib/id.h>
#include <shared/date.h>

#include "author.h"

entity_id_t AuthorRepository_get_next_id() { return generate_entity_id(); };

struct pg_query_params serialize_author_to_pg_params(struct author *author) {
  struct pg_query_params params = new_pg_query_params();

  add_pg_query_param(&params, author->id);
  add_pg_query_param(&params, author->username);
  add_pg_query_param(&params, author->bio);
  add_pg_query_param(&params, author->image);

  char *created_at = unix_timestamp_to_iso8601(author->created_at);
  add_pg_query_param(&params, created_at);
  free(created_at);

  char *updated_at = unix_timestamp_to_iso8601(author->updated_at);
  add_pg_query_param(&params, updated_at);
  free(updated_at);

  return params;
}

enum author_repository_save_error map_query_error_to_author_save_error(PGresult *result) {
  if (has_constraint_violation(result, "author_unique_username"))
    return AUTHOR_REPOSITORY_SAVE_USERNAME_ALREADY_TAKEN;

  return AUTHOR_REPOSITORY_SAVE_APPLICATION_ERROR;
}

struct author_repository_save_result AuthorRepository_save(struct author *author) {
  char *query =
      "INSERT INTO authors (id, username, bio, image, created_at, updated_at) VALUES ($1, $2, $3, $4, $5, $6)";

  struct pg_query_params params = serialize_author_to_pg_params(author);

  PGresult *query_result = pg_query(query, params);

  free_pg_query_params(params);

  struct author_repository_save_result save_result = {
      .success = PQresultStatus(query_result) == PGRES_COMMAND_OK,
  };

  if (!save_result.success) {
    save_result.value.error = map_query_error_to_author_save_error(query_result);
  }

  PQclear(query_result);

  return save_result;
}

struct author_repository_exists_by_id_result AuthorRepository_exists_by_id(entity_id_t author_id) {
  struct pg_query_params params = new_pg_query_params();

  add_pg_query_param(&params, author_id);

  PGresult *query_result = pg_query("SELECT COUNT(*) FROM authors WHERE id = $1", params);

  free_pg_query_params(params);

  struct author_repository_exists_by_id_result exists_result = {
      .success = PQresultStatus(query_result) == PGRES_TUPLES_OK,
  };

  if (exists_result.success) {
    char *count_string = PQgetvalue(query_result, 0, 0);

    exists_result.value.success = atoi(count_string) > 0;
  }

  PQclear(query_result);

  return exists_result;
}
