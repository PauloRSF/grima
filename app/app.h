#ifndef GRIMA_APP_H
#define GRIMA_APP_H

#include <libpq-fe.h>

#include "../http/server.h"
#include "database.h"

typedef struct app_context {
  ServerContext server_context;
  DatabaseContext *database_connection;
} AppContext;

void start_app(int port, AppContext *ctx);

void shutdown_app(AppContext ctx);
#endif
