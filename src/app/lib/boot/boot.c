#include <grima/app.h>

#include "../database/database.h"
#include "../http/http.h"

void start_app(AppContext *app_ctx) {
  connect_database("postgres://postgres:postgres@postgres:5432/grima_development");
  start_http_server(app_ctx);
}

void shutdown_app(AppContext app_ctx) {
  shutdown_server(&app_ctx.server_context);
  // shutdown_database(app_ctx.database_context);
};
