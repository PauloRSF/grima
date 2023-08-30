#ifndef GRIMA_APP_H
#define GRIMA_APP_H

#include <libpq-fe.h>

#include "../http/server.c"

typedef struct app_context {
  ServerContext server_context;
  PGconn *database_connection;
} AppContext;

void start_app(int port);
void shutdown_app(AppContext ctx);
#endif
