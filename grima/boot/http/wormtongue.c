#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cjson/cJSON.h>

#include <cpino.h>
#include <wormtongue/wormtongue.h>

#include <grima/app.h>

#include "http.h"

char *get_request_path_with_query(Request *request) {
  char *path = malloc(strlen(request->path) + 1);

  strcpy(path, request->path);

  if (request->query) {
    char *query_string = get_query_string(request->query);

    path = realloc(path, strlen(request->path) + strlen(query_string) + 2);

    sprintf(path, "%s?%s", path, query_string);

    free(query_string);
  }

  return path;
}

cJSON *get_headers_json(Headers headers) {
  cJSON *headers_json = cJSON_CreateObject();
  size_t index = 0;
  void *item;

  while (headers_iter(headers, &index, &item)) {
    const Header *header = item;

    cJSON_AddStringToObject(headers_json, header->key, header->value);
  }

  return headers_json;
}

bool route_supports_method(Route route, Method method) {
  for (int i = 0; route.methods[i] != -1; i++) {
    if (method == route.methods[i])
      return true;
  }

  return false;
}

void log_request(Request *request) {
  cJSON *request_json = cJSON_CreateObject();

  char request_id[37];

  uuid_unparse_lower(request->id, request_id);

  cJSON_AddStringToObject(request_json, "id", request_id);

  cJSON_AddStringToObject(request_json, "method", get_method_name(request->method));

  char *request_path_with_query = get_request_path_with_query(request);

  cJSON_AddStringToObject(request_json, "url", request_path_with_query);

  free(request_path_with_query);

  cJSON *headers_json = get_headers_json(request->headers);

  cJSON_AddItemToObject(request_json, "headers", headers_json);

  cJSON *context_json = cJSON_CreateObject();

  cJSON_AddItemToObject(context_json, "request", request_json);

  cpino_log_with_context_info(context_json, "HTTP: Request received");

  cJSON_Delete(context_json);
}

void log_response(Request *request, Response *response) {
  cJSON *response_json = cJSON_CreateObject();

  char request_id[37];

  uuid_unparse_lower(request->id, request_id);

  cJSON_AddStringToObject(response_json, "requestId", request_id);

  cJSON_AddNumberToObject(response_json, "statusCode", response->status_code);

  cJSON *headers_json = get_headers_json(response->headers);

  cJSON_AddItemToObject(response_json, "headers", headers_json);

  cJSON *context_json = cJSON_CreateObject();

  cJSON_AddItemToObject(context_json, "response", response_json);

  cpino_log_with_context_info(context_json, "HTTP: Response sent");

  cJSON_Delete(context_json);
}

void ping_controller(AppContext *app_ctx, Request *request, Response *response) {
  response->status_code = 200;

  response->body = "pong";

  add_header(response->headers, "Content-Type", "text/plain");

  send_response(&app_ctx->server_context, request, response);
}

Route routes[] = {{"/ping", (Method[]){GET, -1}, ping_controller}, {NULL, NULL, NULL}};

AppContext *app_ctx1;

void router(WormtongueServerContext *server_ctx, Request *request, Response *response) {
  log_request(request);

  app_ctx1->server_context = *server_ctx;

  for (int i = 0; routes[i].path != NULL; i++) {
    if (MATCH_PATH(request->path, routes[i].path)) {
      if (route_supports_method(routes[i], request->method)) {
        routes[i].handler(app_ctx1, request, response);
      } else {
        response->status_code = 405;
        send_response(server_ctx, request, response);
      }

      break;
    }
  }

  log_response(request, response);
}

bool start_http_server(AppContext *app_ctx) {
  char *port_env_var = getenv("PORT");

  int port = port_env_var != NULL ? atoi(port_env_var) : DEFAULT_PORT;

  cpino_log_info("[HTTP] Starting server");

  app_ctx->server_context = setup_server(port, &printf);

  app_ctx1 = app_ctx;

  start_server(&app_ctx->server_context, router, false);

  cpino_log_info("[HTTP] Listening in port %d", port);

  return true;
}
