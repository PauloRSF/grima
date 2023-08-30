#ifndef GRIMA_PERSONS_APPLICATION_CREATE_PERSON_H
#define GRIMA_PERSONS_APPLICATION_CREATE_PERSON_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cJSON.h>
#include <libpq-fe.h>
#include <uuid/uuid.h>

#include "../../../app.h"
#include "../../../database.h"
#include "../../dates/domain/date.h"
#include "../domain/person.h"
#include "../domain/person_repository.h"

bool validate_person_creation_data(cJSON *person_json, char *error_message) {
  cJSON *id_json = cJSON_GetObjectItemCaseSensitive(person_json, "id");

  if (!id_json) {
    error_message = "You need to provide an id";
    return false;
  }

  if (!cJSON_IsString(id_json) || id_json->valuestring == NULL) {
    error_message = "You need to provde";
    return false;
  }

  return true;
}

Person *create_person_use_case(DatabaseContext *database_context,
                               cJSON *person_json) {

  char *error_message;
  bool is_person_data_valid =
      validate_person_creation_data(person_json, error_message);

  if (!is_person_data_valid) {
    return NULL;
  }

  uuid_t id;
  uuid_generate(id);
  char id_string[37] = {'\0'};
  uuid_unparse_lower(id, id_string);

  cJSON_AddStringToObject(person_json, "id", id_string);

  Person *person = person_from_json(person_json);

  Person *saved_person = person_repository_store(database_context, person);

  free_person(person);

  return saved_person;
};
#endif