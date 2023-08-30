#ifndef GRIMA_APP_H
#define GRIMA_APP_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../http/server.c"
#include "../lib/log.h"

typedef struct app_context {
  ServerContext server_context;
} AppContext;

AppContext ctx;

void handle_ping_endpoint(ServerContext *ctx, Request request,
                          Response response) {
  if (request.method != GET) {
    response.status_code = 405;
    return send_response(ctx, response);
  }

  response.status_code = 200;

  response.body = (char *)calloc(1, 5);
  strcpy(response.body, "pong");

  send_response(ctx, response);
}

void handle_pessoas_endpoint(ServerContext *ctx, Request request,
                             Response response) {
  if (request.method != POST) {
    response.status_code = 405;
    return send_response(ctx, response);
  }

  Header *content_type_header = get_header(request.headers, "Content-Type");

  if (content_type_header == NULL) {
    response.status_code = 400;
    return send_response(ctx, response);
  }

  bool has_json_body =
      strcmp(content_type_header->value, "application/json") == 0;

  free_header(content_type_header);

  if (!has_json_body) {
    response.status_code = 415;
    return send_response(ctx, response);
  }

  response.status_code = 201;

  add_header(response.headers, "Location", "/pessoas/123456d");

  send_response(ctx, response);
}

void handle_request(ServerContext *ctx, Request request, Response response) {
  log_info("%s %s", get_method_name(request.method), request.path);

  add_header(response.headers, "Connection", "close");

  if (strcmp(request.path, "/ping") == 0)
    return handle_ping_endpoint(ctx, request, response);

  if (strcmp(request.path, "/pessoas") == 0)
    return handle_pessoas_endpoint(ctx, request, response);

  response.status_code = 404;

  send_response(ctx, response);
}

void start_app(int port) {
  log_info("Starting server...");

  ctx.server_context = start_server(port);

  log_info("Listening in port %d", port);

  while (true) {
    accept_connection(&ctx.server_context, handle_request);
  }
}

void shutdown_app(AppContext ctx) { shutdown_server(ctx.server_context); };
#endif
