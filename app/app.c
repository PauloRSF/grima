#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libpq-fe.h>
#include <log.h>

#include "app.h"
#include "database.h"
#include "modules/persons/interface/http/persons_count_handler.c"
#include "modules/persons/interface/http/persons_handler.c"

AppContext app_ctx;

void handle_ping_endpoint(ServerContext *server_ctx, Request request,
                          Response response) {
  if (request.method != GET) {
    response.status_code = 405;
    return send_response(server_ctx, response);
  }

  response.status_code = 200;
  response.body = "pong";

  send_response(server_ctx, response);
}

void handle_request(ServerContext *server_ctx, Request request,
                    Response response) {
  log_info("%s %s", get_method_name(request.method), request.path);

  add_header(response.headers, "Connection", "close");

  if (strcmp(request.path, "/ping") == 0)
    return handle_ping_endpoint(server_ctx, request, response);

  if (strncmp(request.path, "/pessoas", strlen("/pessoas")) == 0)
    return persons_handler(&app_ctx, request, response);

  if (strcmp(request.path, "/contagem-pessoas") == 0)
    return persons_count_handler(&app_ctx, request, response);

  response.status_code = 404;

  send_response(server_ctx, response);
}

void start_app(int port, AppContext *ctx) {
  log_info("Connecting to database...");

  app_ctx.database_context = connect_database(getenv("DATABASE_URI"));

  log_info("Starting server...");

  app_ctx.server_context = init_server(port);

  ctx = &app_ctx;

  log_info("Listening in port %d", port);

  start_server(&app_ctx.server_context, handle_request);
}

void shutdown_app(AppContext ctx) {
  shutdown_server(ctx.server_context);
  shutdown_database(ctx.database_context);
};
