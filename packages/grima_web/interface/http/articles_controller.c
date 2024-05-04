#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <grima_core.h>

#include "../../app.h"
#include "article_mapper.h"

void find_articles_handler(AppContext *app_ctx, Request *request, Response *response) {
  FindArticlesResult result = find_articles();

  response->body = FindArticleResultJSONMapper_to_json(result);

  free_find_articles_result(result);

  add_header(response->headers, "Content-Type", "application/json");

  send_response(&app_ctx->server_context, request, response);
}

void articles_controller(AppContext *app_ctx, Request *request, Response *response) {
  if (request->method == GET) {
    return find_articles_handler(app_ctx, request, response);
  }

  response->status_code = 405;

  send_response(&app_ctx->server_context, request, response);
}
