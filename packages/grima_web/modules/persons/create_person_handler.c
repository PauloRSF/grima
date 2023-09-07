#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cJSON.h>
#include <grima_core.h>
#include <grima_http.h>
#include <uuid/uuid.h>

#include "../../app.h"
#include "../../utils/http/http.h"

void create_person_handler(AppContext *app_ctx, RequestContext *request_ctx,
                           Request *request, Response *response) {
  char *content_type = get_header_value(request->headers, "Content-Type");

  if (content_type == NULL) {
    response->status_code = 400;
    return send_response(&app_ctx->server_context, request_ctx, response);
  }

  bool has_json_body = strcmp(content_type, "application/json") == 0;

  if (!has_json_body) {
    response->status_code = 415;
    return send_response(&app_ctx->server_context, request_ctx, response);
  }

  cJSON *person_json = cJSON_Parse(request->body);

  if (person_json == NULL) {
    cJSON_Delete(person_json);
    response->status_code = 422;
    return send_response(&app_ctx->server_context, request_ctx, response);
  }

  Error *error = NULL;

  Person *saved_person =
      create_person_use_case(app_ctx->database_context, person_json, &error);

  cJSON_Delete(person_json);

  if (saved_person == NULL) {
    if (error) {
      get_response_for_error(error, response);
      free_error(error);
    } else {
      response->status_code = 500;
    }

    return send_response(&app_ctx->server_context, request_ctx, response);
  }

  response->status_code = 201;

  char saved_person_uuid[UUID_STR_LEN];
  uuid_unparse_lower(saved_person->id, saved_person_uuid);

  char *endpoint_path = "/pessoas/";
  char *location_response_header_value =
      malloc(strlen(endpoint_path) + strlen(saved_person_uuid) + 1);

  sprintf(location_response_header_value, "%s%s", endpoint_path,
          saved_person_uuid);

  response->body = person_to_json(saved_person);
  free_person(saved_person);

  add_header(response->headers, "Location", location_response_header_value);
  add_header(response->headers, "Content-Type", "application/json");

  free(location_response_header_value);

  send_response(&app_ctx->server_context, request_ctx, response);
}
