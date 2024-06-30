#include <stdbool.h>

#include <libpq-fe.h>

bool connect_database(char *connection_uri);

void shutdown_database(PGconn *connection);
