#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cJSON.h>
#include <grima_core.h>
#include <uuid/uuid.h>
#include <yuarel.h>

#include "../../app.h"

void find_articles_handler(AppContext *app_ctx, RequestContext *request_ctx,
                          Request *request, Response *response) {
  ArticlesSearchResult result =
      find_articles(app_ctx->database_context, search_term);

  response->body = articles_to_json(result.articles, result.articles_count);

  free_article_search_result(&result);

  add_header(response->headers, "Content-Type", "application/json");

  send_response(&app_ctx->server_context, request_ctx, response);
}

void articles_controller(AppContext *app_ctx, RequestContext *request_ctx,
                     Request *request, Response *response) {
  if (request->method == GET) {
    return find_articles_handler(app_ctx, request_ctx, request, response);
  }

  response->status_code = 405;
  send_response(&app_ctx->server_context, request_ctx, response);
}
