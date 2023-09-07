#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cJSON.h>
#include <grima_core.h>
#include <uuid/uuid.h>

#include "../../app.h"

void persons_count_handler(AppContext *app_ctx, RequestContext *request_ctx,
                           Request *request, Response *response) {
  if (request->method != GET) {
    response->status_code = 405;
    return send_response(&app_ctx->server_context, request_ctx, response);
  }

  size_t count = count_persons_use_case(app_ctx->database_context);

  add_header(response->headers, "Content-Type", "text/plain");

  char count_str[33];
  sprintf(count_str, "%lu", count);
  response->body = malloc(strlen(count_str) + 1);
  strcpy(response->body, count_str);

  return send_response(&app_ctx->server_context, request_ctx, response);
}
