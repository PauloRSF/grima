#ifndef GRIMA_CORE_DATABASE_H
#define GRIMA_CORE_DATABASE_H

#include <libpq-fe.h>

#define DATABASE_LOG_TAG "[DATABASE]"

PGconn *get_database_connection();

void connect_database(char *connection_uri);

void shutdown_database(PGconn *ctx);

#endif