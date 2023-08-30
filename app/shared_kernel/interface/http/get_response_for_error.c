#include <cJSON.h>
#include <string_list.h>

#include "http.h"

void get_response_for_error(Error *error, Response *response) {
  if (error->kind == ValidationErrorKind) {
    ValidationError *validation_error = (ValidationError *)error;
    response->status_code = 422;

    cJSON *response_json = cJSON_CreateObject();

    cJSON_AddStringToObject(response_json, "message",
                            validation_error->message);

    cJSON *details_array_json =
        cJSON_AddArrayToObject(response_json, "details");

    if (!StringList_is_empty(validation_error->details)) {
      StringListNode *item;

      StringList_ForEach(item, validation_error->details) {
        cJSON *detail_message = cJSON_CreateString(item->data);
        cJSON_AddItemToArray(details_array_json, detail_message);
      }
    }

    response->body = cJSON_Print(response_json);
  } else {
    response->status_code = 500;
  }
}
