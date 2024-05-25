#include <stdlib.h>

#include <libpq-fe.h>
#include <cpino.h>

#include "database.h"

PGconn *connection = NULL;

PGconn *get_database_connection() {
  return connection;
}

void connect_database(char *connection_uri) {
  cpino_log_info("Connecting to database...");

  connection = PQconnectdb(connection_uri);

  if (PQstatus(connection) != CONNECTION_OK) {
    cpino_log_error("Could not connect to database: %s", PQerrorMessage(connection));
    PQfinish(connection);
    exit(9);
  }
}

void shutdown_database(PGconn *ctx) { PQfinish(ctx); }
