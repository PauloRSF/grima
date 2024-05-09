#ifndef GRIMA_APP_H
#define GRIMA_APP_H

#include <wormtongue.h>

// #include "database.h"

typedef struct app_context {
  WormtongueServerContext server_context;
  // DatabaseContext *database_context;
} AppContext;

void start_app(AppContext *ctx);

void shutdown_app(AppContext ctx);

#endif
