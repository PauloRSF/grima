#include <stdlib.h>

#include <libpq-fe.h>
#include <log.h>

#include "database.h"

DatabaseContext *connect_database(char *connection_uri) {
  PGconn *connection = PQconnectdb(connection_uri);

  if (PQstatus(connection) != CONNECTION_OK) {
    log_error("Could not connect to database: %s", PQerrorMessage(connection));
    PQfinish(connection);
    exit(9);
  }

  return connection;
}

void shutdown_database(DatabaseContext *ctx) { PQfinish(ctx); }
