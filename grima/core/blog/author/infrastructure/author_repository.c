#include <stdlib.h>
#include <string.h>

#include <cpino.h>
#include <libpq-fe.h>
#include <uuid/uuid.h>

#include <lib/database.h>
#include <shared/date.h>

#include "../author.h"

#define AUTHOR_PARAMS_COUNT 6

char *AuthorRepository_get_next_id() {
  uuid_t author_id;

  uuid_generate(author_id);

  char *author_id_string = malloc(37 * sizeof(char));

  uuid_unparse(author_id, author_id_string);

  return author_id_string;
};

struct author_params {
  const char *values[AUTHOR_PARAMS_COUNT];
  int lengths[AUTHOR_PARAMS_COUNT];
  int formats[AUTHOR_PARAMS_COUNT];
};

struct author_params build_author_params(struct author *author) {
  struct author_params params = {
      .values = {0},
      .lengths = {0},
      .formats = {0},
  };

  params.values[0] = author->id;
  params.lengths[0] = strlen(author->id);
  params.formats[0] = 0;

  params.values[1] = author->username;
  params.lengths[1] = strlen(author->username);
  params.formats[1] = 0;

  params.values[2] = author->bio;
  params.lengths[2] = author->bio ? strlen(author->bio) : 0;
  params.formats[2] = 0;

  params.values[3] = author->image;
  params.lengths[3] = author->image ? strlen(author->image) : 0;
  params.formats[3] = 0;

  params.values[4] = calloc(ISO_TIME_LENGTH, sizeof(char));
  unix_timestamp_to_iso8601(author->updated_at, (char *)params.values[4]);
  params.lengths[4] = ISO_TIME_LENGTH;
  params.formats[4] = 0;

  params.values[5] = calloc(ISO_TIME_LENGTH, sizeof(char));
  unix_timestamp_to_iso8601(author->updated_at, (char *)params.values[5]);
  params.lengths[5] = ISO_TIME_LENGTH;
  params.formats[5] = 0;

  return params;
}

enum author_repository_save_error map_postgres_error_to_domain_error(PGresult *result) {
  char *error_type = PQresultErrorField(result, PG_DIAG_SQLSTATE);

  bool is_unique_constraint_violation =
      strcmp(error_type, POSTGRES_CONSTRAINT_VIOLATION_ERROR_CODE) == 0;

  if (!is_unique_constraint_violation)
    return AUTHOR_REPOSITORY_SAVE_APPLICATION_ERROR;

  char *constraint_name = PQresultErrorField(result, PG_DIAG_CONSTRAINT_NAME);

  if (strcmp(constraint_name, "author_unique_username") == 0)
    return AUTHOR_REPOSITORY_SAVE_USERNAME_ALREADY_TAKEN;

  return AUTHOR_REPOSITORY_SAVE_APPLICATION_ERROR;
}

struct author_repository_save_result AuthorRepository_save(struct author *author) {
  PGconn *connection = get_database_connection();

  const char *query = "INSERT INTO authors (id, username, bio, image, created_at, updated_at) "
                      "VALUES ($1, $2, $3, $4, $5, $6)";

  struct author_params params = build_author_params(author);

  cpino_log_debug("[DATABASE] %s", query);

  PGresult *result = PQexecParams(connection, query, AUTHOR_PARAMS_COUNT, NULL, params.values,
                                  params.lengths, params.formats, 0);

  if (PQresultStatus(result) == PGRES_COMMAND_OK) {
    PQclear(result);

    return (struct author_repository_save_result){
        .success = true,
    };
  }

  char *message = PQresultVerboseErrorMessage(result, PQERRORS_VERBOSE, PQSHOW_CONTEXT_ALWAYS);

  cpino_log_error("[DATABASE] Failed to insert Author: %s", message);

  PQfreemem(message);

  struct author_repository_save_result error_result = {
      .success = false,
  };

  error_result.value.error = map_postgres_error_to_domain_error(result);

  PQclear(result);

  return error_result;
}
