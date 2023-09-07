#ifndef GRIMA_APP_H
#define GRIMA_APP_H

#include <grima_http.h>
#include <libpq-fe.h>

#include "database.h"

typedef struct app_context {
  ServerContext server_context;
  DatabaseContext *database_context;
} AppContext;

void start_app(int port, AppContext *ctx);

void shutdown_app(AppContext ctx);
#endif
