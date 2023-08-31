#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cJSON.h>
#include <uuid/uuid.h>

#include "../../../../../http/server.h"
#include "../../../../app.h"
#include "../../../../shared_kernel/interface/http/http.h"
#include "../../application/create_person.c"
#include "../../domain/person.h"
#include "../../domain/person_repository.h"

void create_person_handler(AppContext *app_ctx, Request *request,
                           Response *response) {
  char *content_type = get_header_value(request->headers, "Content-Type");

  if (content_type == NULL) {
    response->status_code = 400;
    return send_response(&app_ctx->server_context, response);
  }

  bool has_json_body = strcmp(content_type, "application/json") == 0;

  if (!has_json_body) {
    response->status_code = 415;
    return send_response(&app_ctx->server_context, response);
  }

  cJSON *person_json = cJSON_Parse(request->body);

  if (person_json == NULL) {
    cJSON_Delete(person_json);
    response->status_code = 422;
    return send_response(&app_ctx->server_context, response);
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

    return send_response(&app_ctx->server_context, response);
  }

  response->status_code = 201;

  char saved_person_uuid[37];
  uuid_unparse_lower(saved_person->id, saved_person_uuid);

  char *endpoint_path = "/pessoas/";
  char *location_response_header_value =
      malloc(strlen(endpoint_path) + strlen(saved_person_uuid) + 1);

  sprintf(location_response_header_value, "%s%s", endpoint_path,
          saved_person_uuid);

  free_person(saved_person);

  add_header(response->headers, "Location", location_response_header_value);
  add_header(response->headers, "Content-Type", "application/json");

  free(location_response_header_value);

  send_response(&app_ctx->server_context, response);
}
