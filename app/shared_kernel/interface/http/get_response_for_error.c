#include <stdlib.h>
#include <string.h>

#include <cJSON.h>
#include <string_list.h>

#include "http.h"

void get_response_for_error(Error *error, Response *response) {
  if (error->kind == ValidationErrorKind) {
    ValidationError *validation_error = (ValidationError *)error;
    response->status_code = 422;

    cJSON *response_json = cJSON_CreateObject();

    char *dup_message = malloc(strlen(validation_error->message) + 1);
    strcpy(dup_message, validation_error->message);

    cJSON_AddStringToObject(response_json, "message", dup_message);

    cJSON *details_array_json =
        cJSON_AddArrayToObject(response_json, "details");

    if (!StringList_is_empty(validation_error->details)) {
      StringList *dup = StringList_clone(validation_error->details);
      StringListNode *item;

      StringList_ForEach(item, dup) {
        cJSON_AddItemToArray(details_array_json,
                             cJSON_CreateString(item->data));
      }

      StringList_free(dup);
    }

    free(response->body);

    response->body = cJSON_Print(response_json);

    cJSON_Delete(response_json);
  } else {
    response->status_code = 500;
  }
}
