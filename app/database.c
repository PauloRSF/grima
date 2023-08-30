#include <stdlib.h>

#include <libpq-fe.h>
#include <log.h>

PGconn *connect_database() {
  PGconn *connection = PQconnectdb(getenv("DATABASE_URI"));

  if (PQstatus(connection) != CONNECTION_OK) {
    log_error("Could not connect to database: %s", PQerrorMessage(connection));
    PQfinish(connection);
    exit(9);
  }

  return connection;
}

void shutdown_database(PGconn *connection) { PQfinish(connection); }
