#include <stdlib.h>
#include <string.h>

#include <sys/random.h>

#include <cpino.h>
#include <libpq-fe.h>
#include <uuid/uuid.h>

#include <lib/database.h>
#include <shared/date.h>

#include "../account.h"

#define SESSION_PARAMS_COUNT 4

void SessionRepository_get_next_id(session_id_t session_id) {
  size_t id_bytes_count = sizeof(session_id_t) / 2;
  char id_bytes[id_bytes_count];

  getrandom(id_bytes, id_bytes_count, 0);

  for (size_t i = 0; i < id_bytes_count; i++) {
    sprintf(session_id + (i * 2), "%02x", id_bytes[i]);
  }

  session_id[63] = '\0';
};

struct session_params {
  const char *values[SESSION_PARAMS_COUNT];
  int lengths[SESSION_PARAMS_COUNT];
  int formats[SESSION_PARAMS_COUNT];
};

struct session_params build_session_params(struct session *session) {
  struct session_params params = {
      .values = {0},
      .lengths = {0},
      .formats = {0},
  };

  params.values[0] = session->id;
  params.lengths[0] = 16;
  params.formats[0] = 1;

  params.values[1] = session->account_id;
  params.lengths[1] = 16;
  params.formats[1] = 1;

  params.values[2] = calloc(ISO_TIME_LENGTH, sizeof(char));
  unix_timestamp_to_iso8601(session->created_at, (char *)params.values[2]);
  params.lengths[2] = ISO_TIME_LENGTH;
  params.formats[2] = 0;

  params.values[3] = calloc(ISO_TIME_LENGTH, sizeof(char));
  unix_timestamp_to_iso8601(session->expires_at, (char *)params.values[3]);
  params.lengths[3] = ISO_TIME_LENGTH;
  params.formats[3] = 0;

  return params;
}

struct session_repository_save_result SessionRepository_save(struct session *session) {
  PGconn *connection = get_database_connection();

  const char *query = "INSERT INTO sessions (id, account_id, created_at, expires_at) "
                      "VALUES ($1, $2, $3, $4)";

  struct session_params params = build_session_params(session);

  cpino_log_debug("[DATABASE] %s", query);

  PGresult *result =
      PQexecParams(connection, query, SESSION_PARAMS_COUNT, NULL, params.values,
                   params.lengths, params.formats, 0);

  if (PQresultStatus(result) == PGRES_COMMAND_OK) {
    PQclear(result);

    return (struct session_repository_save_result){
        .success = true,
    };
  }

  char *message = PQresultVerboseErrorMessage(result, PQERRORS_VERBOSE,
                                              PQSHOW_CONTEXT_ALWAYS);

  cpino_log_error("[DATABASE] Failed to insert Session: %s", message);

  PQfreemem(message);

  PQclear(result);
  
  struct session_repository_save_result error_result = {
    .success = false,
  };

  error_result.value.error = SESSION_REPOSITORY_SAVE_APPLICATION_ERROR;

  return error_result;
}
