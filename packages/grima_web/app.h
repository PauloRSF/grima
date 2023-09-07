#ifndef GRIMA_APP_H
#define GRIMA_APP_H

#include <grima_http.h>
#include <libpq-fe.h>

#include "database.h"

typedef struct app_context {
  ServerContext server_context;
  DatabaseContext *database_context;
} AppContext;

#define MATCH_PATH(request_path, path)                                         \
  strncmp((request_path), (path), strlen((path))) == 0

void start_app(int port, AppContext *ctx);

void shutdown_app(AppContext ctx);

#endif
