#ifndef GRIMA_APP_H
#define GRIMA_APP_H

#include "../http/server.c"

typedef struct app_context {
  ServerContext server_context;
} AppContext;

void start_app(int port);
void shutdown_app(AppContext ctx);
#endif
