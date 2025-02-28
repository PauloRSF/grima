#include <stdlib.h>

#include <cpino.h>
#include <libpq-fe.h>

PGconn *database_connection = NULL;

PGconn *get_database_connection() {
  if (!database_connection) {
    cpino_log_error("[DATABASE] Database is not connected");
    exit(9);
  } else if (PQstatus(database_connection) != CONNECTION_OK) {
    cpino_log_error("[DATABASE] Database connection is not healthy");
    exit(9);
  }

  return database_connection;
}

void set_database_connection(PGconn *connection) { database_connection = connection; }
