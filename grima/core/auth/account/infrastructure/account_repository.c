#include <string.h>

#include <cpino.h>
#include <libpq-fe.h>
#include <uuid/uuid.h>

#include <lib/database.h>

#include "../account.h"

void AccountRepository_get_next_id(AccountId account_id) { uuid_generate(account_id); };

#define ACCOUNT_PARAMS_COUNT 5

struct account_params {
  const char *values[ACCOUNT_PARAMS_COUNT];
  int lengths[ACCOUNT_PARAMS_COUNT];
  int formats[ACCOUNT_PARAMS_COUNT];
};

struct account_params build_account_params(Account *account) {
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
  params.lengths[3] = PASSWORD_SALT_SIZE;
  params.formats[3] = 1;

  params.values[4] = account->password.hash;
  params.lengths[4] = PASSWORD_HASH_SIZE;
  params.formats[4] = 1;

  return params;
}

void AccountRepository_save(Account *account) {
  PGconn *connection = get_database_connection();

  const char *query = "INSERT INTO accounts (id, email, username, password_salt, password_hash) "
                      "VALUES ($1, $2, $3, $4, $5)";

  struct account_params params = build_account_params(account);

  cpino_log_debug("[DATABASE] %s", query);

  PGresult *result = PQexecParams(connection, query, ACCOUNT_PARAMS_COUNT, NULL, params.values,
                                  params.lengths, params.formats, 0);

  if (PQresultStatus(result) != PGRES_TUPLES_OK) {
    cpino_log_error("[DATABASE] Failed to insert Account: %s", PQerrorMessage(connection));
  }

  PQclear(result);
}
