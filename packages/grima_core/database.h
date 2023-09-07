#ifndef GRIMA_CORE_DATABASE_H
#define GRIMA_CORE_DATABASE_H

#include <libpq-fe.h>

#define DATABASE_LOG_TAG "[DATABASE]"

typedef PGconn DatabaseContext;

DatabaseContext *connect_database(char *connection_uri);

void shutdown_database(DatabaseContext *ctx);

#endif