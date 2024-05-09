#include <cpino.h>

#include <grima_app.h>

// #include "../../database.h"
#include "../http/http.h"

void start_app(AppContext *app_ctx) {
  // cpino_log_info("Connecting to database...");

  // app_ctx.database_context = connect_database(getenv("DATABASE_URI"));

  start_http_server(app_ctx);
}

void shutdown_app(AppContext app_ctx) {
  shutdown_server(&app_ctx.server_context);
  // shutdown_database(app_ctx.database_context);
};
