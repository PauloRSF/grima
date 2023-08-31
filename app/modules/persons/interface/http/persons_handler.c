#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cJSON.h>
#include <uuid/uuid.h>
#include <yuarel.h>

#include "../../../../app.h"
#include "../../application/create_person.c"
#include "../../application/get_person_by_id.c"
#include "../../domain/person.h"
#include "../../domain/person_repository.h"
#include "create_person_handler.c"
#include "find_persons_handler.c"
#include "get_person_handler.c"

void persons_handler(AppContext *app_ctx, Request *request,
                     Response *response) {
  if (request->method == GET) {
    char *path_segments[4] = {NULL};

    int path_segments_count =
        yuarel_split_path(request->path, path_segments, 3);

    if (path_segments_count == -1) {
      response->status_code = 400;
      return send_response(&app_ctx->server_context, response);
    }

    if (path_segments_count == 2) {
      return get_person_handler(app_ctx, request, response);
    }

    response->status_code = 404;
    return send_response(&app_ctx->server_context, response);
  }

  if (request->method == POST)
    return create_person_handler(app_ctx, request, response);

  response->status_code = 405;
  send_response(&app_ctx->server_context, response);
}
