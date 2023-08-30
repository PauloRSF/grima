#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <log.h>

#include "app.h"

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
    return handle_persons_endpoint(&app_ctx, request, response);

  response.status_code = 404;

  send_response(server_ctx, response);
}

void start_app(int port) {
  log_info("Starting server...");

  app_ctx.server_context = start_server(port);

  log_info("Listening in port %d", port);

  while (true) {
    accept_connection(&app_ctx.server_context, handle_request);
  }
}

void shutdown_app(AppContext ctx) { shutdown_server(ctx.server_context); };
