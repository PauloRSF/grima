#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cJSON.h>
#include <string_map.h>
#include <uuid/uuid.h>

#include "../../../../app.h"
#include "../../application/find_persons.c"
#include "../../application/get_person_by_id.c"
#include "../../domain/person.h"

void find_persons_handler(AppContext *app_ctx, Request *request,
                          Response *response) {
  char *search_term = StringMap_get(request->query, "t");

  if (search_term == NULL) {
    response->status_code = 400;
    send_response(&app_ctx->server_context, response);
  }

  PersonSearchResult result =
      find_persons_use_case(app_ctx->database_context, search_term);

  response->body = persons_to_json(result.persons, result.persons_count);

  free_person_search_result(&result);

  add_header(response->headers, "Content-Type", "application/json");

  send_response(&app_ctx->server_context, response);
}
