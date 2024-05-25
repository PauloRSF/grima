#include <string.h>

#include <grima/app.h>
#include <grima/articles.h>

#include <wormtongue/headers.h>
#include <wormtongue/query.h>

#include "article_mapper.h"

void find_articles_handler(AppContext *app_ctx, Request *request, Response *response) {
  FindArticlesOptions options = {
    .tag = NULL,
    .author = NULL,
    .favorited = NULL,
    .limit = 10,
    .offset = 0,
  };

  char* tag = get_query_param_value(request->query, "tag");
  if (tag && strlen(tag) > 0) options.tag = tag;

  char* author = get_query_param_value(request->query, "author");
  if (author && strlen(author) > 0) options.author = author;

  char* favorited = get_query_param_value(request->query, "favorited");
  if (favorited && strlen(favorited) > 0) options.favorited = favorited;

  char* limit = get_query_param_value(request->query, "limit");
  if (limit && strlen(limit) > 0) {
    size_t limit_value = atoi(limit);
    if (limit_value > 0) options.limit = limit_value;
  }

  char* offset = get_query_param_value(request->query, "offset");
  if (offset && strlen(offset) > 0) {
    size_t offset_value = atoi(offset);
    if (offset_value > 0) options.offset = offset_value;
  }

  FindArticlesResult result = find_articles(options);

  if (result.is_error) {
    response->status_code = 500;

    send_response(&app_ctx->server_context, request, response);

    return;
  }

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
