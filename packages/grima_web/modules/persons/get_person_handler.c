#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cJSON.h>
#include <grima_core.h>
#include <uuid/uuid.h>

#include "../../app.h"

void get_person_handler(AppContext *app_ctx, RequestContext *request_ctx,
                        Request *request, Response *response) {
  uuid_t id;
  char id_param[UUID_STR_LEN] = {'\0'};
  strncpy(id_param, request->path + sizeof("pessoas/"), 36);
  uuid_parse(id_param, id);

  Person *person = get_person_by_id_use_case(app_ctx->database_context, id);

  if (!person) {
    response->status_code = 404;
    return send_response(&app_ctx->server_context, request_ctx, response);
  }

  response->body = person_to_json(person);

  free_person(person);

  add_header(response->headers, "Content-Type", "application/json");

  send_response(&app_ctx->server_context, request_ctx, response);
}
