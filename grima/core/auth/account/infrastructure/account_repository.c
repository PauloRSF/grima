#include <stdlib.h>
#include <string.h>

#include <cpino.h>
#include <libpq-fe.h>
#include <uuid/uuid.h>

#include <lib/database.h>
#include <shared/date.h>

#include "../account.h"

#define ACCOUNT_PARAMS_COUNT 7
#define POSTGRES_CONSTRAINT_VIOLATION_ERROR_CODE "23505"

void AccountRepository_get_next_id(account_id_t account_id) {
  uuid_generate(account_id);
};

struct account_params {
  const char *values[ACCOUNT_PARAMS_COUNT];
  int lengths[ACCOUNT_PARAMS_COUNT];
  int formats[ACCOUNT_PARAMS_COUNT];
};

struct account_params build_account_params(struct account *account) {
  struct account_params params = {
      .values = {0},
      .lengths = {0},
      .formats = {0},
  };

  params.values[0] = account->id;
  params.lengths[0] = 16;
  params.formats[0] = 1;

  params.values[1] = account->email;
  params.lengths[1] = strlen(account->email);
  params.formats[1] = 0;

  params.values[2] = account->username;
  params.lengths[2] = strlen(account->username);
  params.formats[2] = 0;

  params.values[3] = account->password.salt;
  params.lengths[3] = ACCOUNT_PASSWORD_SALT_SIZE;
  params.formats[3] = 1;

  params.values[4] = account->password.hash;
  params.lengths[4] = ACCOUNT_PASSWORD_HASH_SIZE;
  params.formats[4] = 1;

  params.values[5] = calloc(ISO_TIME_LENGTH, sizeof(char));
  unix_timestamp_to_iso8601(account->updated_at, (char *)params.values[5]);
  params.lengths[5] = ISO_TIME_LENGTH;
  params.formats[5] = 0;

  params.values[6] = calloc(ISO_TIME_LENGTH, sizeof(char));
  unix_timestamp_to_iso8601(account->updated_at, (char *)params.values[6]);
  params.lengths[6] = ISO_TIME_LENGTH;
  params.formats[6] = 0;

  return params;
}

enum account_repository_save_error
map_postgres_error_to_domain_error(PGresult *result) {
  char *error_type = PQresultErrorField(result, PG_DIAG_SQLSTATE);

  bool is_unique_constraint_violation =
      strcmp(error_type, POSTGRES_CONSTRAINT_VIOLATION_ERROR_CODE) == 0;

  if (!is_unique_constraint_violation) {
    return ACCOUNT_REPOSITORY_SAVE_APPLICATION_ERROR;
  }

  char *constraint_name = PQresultErrorField(result, PG_DIAG_CONSTRAINT_NAME);

  if (strcmp(constraint_name, "account_unique_email") == 0) {
    return ACCOUNT_REPOSITORY_SAVE_EMAIL_ALREADY_TAKEN;
  }

  if (strcmp(constraint_name, "account_unique_username") == 0) {
    return ACCOUNT_REPOSITORY_SAVE_USERNAME_ALREADY_TAKEN;
  }

  return ACCOUNT_REPOSITORY_SAVE_APPLICATION_ERROR;
}

struct account_repository_save_result
AccountRepository_save(struct account *account) {
  PGconn *connection = get_database_connection();

  const char *query = "INSERT INTO accounts (id, email, username, "
                      "password_salt, password_hash, "
                      "created_at, updated_at) "
                      "VALUES ($1, $2, $3, $4, $5, $6, $7)";

  struct account_params params = build_account_params(account);

  cpino_log_debug("[DATABASE] %s", query);

  PGresult *result =
      PQexecParams(connection, query, ACCOUNT_PARAMS_COUNT, NULL, params.values,
                   params.lengths, params.formats, 0);

  if (PQresultStatus(result) == PGRES_COMMAND_OK) {
    PQclear(result);

    return (struct account_repository_save_result){
        .success = true,
    };
  }

  char *message = PQresultVerboseErrorMessage(result, PQERRORS_VERBOSE,
                                              PQSHOW_CONTEXT_ALWAYS);

  cpino_log_error("[DATABASE] Failed to insert Account: %s", message);

  PQfreemem(message);

  enum account_repository_save_error save_error =
      map_postgres_error_to_domain_error(result);

  PQclear(result);
  
  struct account_repository_save_result error_result = {
    .success = false,
  };

  error_result.value.error = save_error;

  return error_result;
}

bool AccountRepository_exists_by_id(account_id_t account_id) {
  PGconn *connection = get_database_connection();

  const char *query = "SELECT COUNT(*) FROM accounts WHERE id = $1";

  const char *values[1] = {account_id};
  int lengths[1] = {16};
  int formats[1] = {1};

  cpino_log_debug("[DATABASE] %s", query);

  PGresult *result =
      PQexecParams(connection, query, 1, NULL, values, lengths, formats, 0);

  if (PQresultStatus(result) != PGRES_TUPLES_OK) {
    char *message = PQresultVerboseErrorMessage(result, PQERRORS_VERBOSE,
                                                PQSHOW_CONTEXT_ALWAYS);

    cpino_log_error("[DATABASE] Failed to check if Account exists: %s", message);

    PQfreemem(message);

    PQclear(result);

    return false;
  }

  char *count = PQgetvalue(result, 0, 0);

  bool exists = strcmp(count, "1") == 0;

  PQclear(result);

  return exists;
}
