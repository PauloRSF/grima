#ifndef GRIMA_APP_DATABASE_H
#define GRIMA_APP_DATABASE_H

#include <libpq-fe.h>

typedef PGconn DatabaseContext;

DatabaseContext *connect_database(char *connection_uri);

void shutdown_database(DatabaseContext *ctx);
#endif