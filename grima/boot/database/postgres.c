#include <stdbool.h>
#include <stdlib.h>

#include <cpino.h>
#include <libpq-fe.h>

#include <lib/database.h>

#include "database.h"

bool connect_database(char *connection_uri) {
  cpino_log_info("[DATABASE] Connecting to database");

  PGconn *connection = connection = PQconnectdb(connection_uri);

  if (PQstatus(connection) != CONNECTION_OK) {
    cpino_log_error("Could not connect to database: %s", PQerrorMessage(connection));
    PQfinish(connection);

    return false;
  }

  cpino_log_info("[DATABASE] Database connected");

  const char *query = "SELECT 1+1";

  cpino_log_debug("[DATABASE] %s", "SELECT 1+1");

  PGresult *result = PQexecParams(connection, query, 0, NULL, NULL, NULL, NULL, 0);

  if (PQresultStatus(result) != PGRES_TUPLES_OK) {
    PQclear(result);

    char *message = PQresultVerboseErrorMessage(result, PQERRORS_VERBOSE, PQSHOW_CONTEXT_ALWAYS);

    cpino_log_error("[DATABASE] Database is not healthy: %s", message);

    return false;
  }

  set_database_connection(connection);

  return true;
}

void shutdown_database(PGconn *connection) { PQfinish(connection); }
