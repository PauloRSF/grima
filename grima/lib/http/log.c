#include <stdio.h>
#include <stdlib.h>

#include <uuid/uuid.h>

#include <cjson/cJSON.h>
#include <cpino.h>
#include <wormtongue/wormtongue.h>

char *get_request_path_with_query(Request *request) {
  char *path = NULL;

  if (!request->query) {
    asprintf(&path, "%s", request->path);
    return path;
  }

  char *query_string = get_query_string(request->query);

  asprintf(&path, "%s?%s", request->path, query_string);

  free(query_string);

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

void log_request(Request *request) {
  cJSON *request_json = cJSON_CreateObject();

  char request_id[37];

  uuid_unparse_lower(request->id, request_id);

  cJSON_AddStringToObject(request_json, "id", request_id);

  cJSON_AddStringToObject(request_json, "method",
                          get_method_name(request->method));

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
