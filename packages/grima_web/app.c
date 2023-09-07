#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <grima_http.h>
#include <libpq-fe.h>
#include <log.h>

#include "app.h"
#include "database.h"
#include "modules/persons/persons_count_handler.c"
#include "modules/persons/persons_handler.c"

AppContext app_ctx;

void handle_ping_endpoint(ServerContext *server_ctx,
                          RequestContext *request_ctx, Request *request,
                          Response *response) {
  if (request->method != GET) {
    response->status_code = 405;
    return send_response(server_ctx, request_ctx, response);
  }

  response->status_code = 200;
  response->body = "pong";

  send_response(server_ctx, request_ctx, response);
}

void log_request(Request *request) {
  if (request->query) {
    char *query_string = get_query_string(request->query);

    log_info("%s %s?%s", get_method_name(request->method), request->path,
             query_string);

    free(query_string);
  } else {

    log_info("%s %s", get_method_name(request->method), request->path);
  }
}

void log_response(Response *response) {
  log_info("%d %s", response->status_code,
           get_status_text(response->status_code));
}

void handle_request(ServerContext *server_ctx, RequestContext *request_ctx,
                    Request *request, Response *response) {
  log_request(request);

  app_ctx.server_context = *server_ctx;

  if (MATCH_PATH(request->path, "/ping"))
    handle_ping_endpoint(server_ctx, request_ctx, request, response);
  else if (MATCH_PATH(request->path, "/pessoas"))
    persons_handler(&app_ctx, request_ctx, request, response);
  else if (MATCH_PATH(request->path, "/contagem-pessoas"))
    persons_count_handler(&app_ctx, request_ctx, request, response);
  else {
    response->status_code = 404;
    send_response(server_ctx, request_ctx, response);
  }

  log_response(response);
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
  shutdown_server(&ctx.server_context);
  shutdown_database(ctx.database_context);
};
