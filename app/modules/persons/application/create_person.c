#ifndef GRIMA_PERSONS_APPLICATION_CREATE_PERSON_H
#define GRIMA_PERSONS_APPLICATION_CREATE_PERSON_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cJSON.h>
#include <libpq-fe.h>
#include <string_list.h>
#include <uuid/uuid.h>

#include "../../../app.h"
#include "../../../database.h"
#include "../../../shared_kernel/dates/date.h"
#include "../../../shared_kernel/error/error.h"
#include "../domain/person.h"
#include "../domain/person_repository.h"

void add_error_message(char ***error_messages, const char *message,
                       size_t *count) {
  char **new_error_messages =
      realloc(*error_messages, (*count + 1) * sizeof(char *));

  new_error_messages[*count] = (char *)message;

  (*count)++;

  *error_messages = new_error_messages;
}

bool validate_person_creation_data(DatabaseContext *database_context,
                                   cJSON *person_json, Error **error) {
  StringList *details = StringList_new();

  cJSON *name_json = cJSON_GetObjectItemCaseSensitive(person_json, "nome");

  if (!name_json) {
    StringList_add(details, "'nome' is required");
  } else if (!cJSON_IsString(name_json)) {
    StringList_add(details, "'nome' must be a string");
  }

  cJSON *nickname_json =
      cJSON_GetObjectItemCaseSensitive(person_json, "apelido");

  if (!nickname_json) {
    StringList_add(details, "'apelido' is required");
  } else if (!cJSON_IsString(nickname_json)) {
    StringList_add(details, "'apelido' must be a string");
  } else {
    bool is_nickname_taken = person_repository_is_nickname_taken(
        database_context, nickname_json->valuestring);

    if (is_nickname_taken)
      StringList_add(details, "'apelido' is already taken");
  }

  cJSON *date_of_birth_json =
      cJSON_GetObjectItemCaseSensitive(person_json, "nascimento");

  if (!date_of_birth_json) {
    StringList_add(details, "'nascimento' is required");
  } else if (!cJSON_IsString(date_of_birth_json)) {
    StringList_add(details, "'nascimento' must be a string");
  } else if (!is_well_formatted_date_string(date_of_birth_json->valuestring)) {
    StringList_add(details, "'nascimento' must be in YYYY-MM-DD format");
  } else if (!is_valid_date_string(date_of_birth_json->valuestring)) {
    StringList_add(details, "'nascimento' must be a valid date");
  }

  cJSON *stack_json = cJSON_GetObjectItemCaseSensitive(person_json, "stack");

  if (stack_json && !cJSON_IsNull(stack_json)) {
    if (!cJSON_IsArray(stack_json)) {
      StringList_add(details, "'stack' must be an array");
    } else {
      cJSON *item;

      cJSON_ArrayForEach(item, stack_json) {
        if (!cJSON_IsString(item)) {
          StringList_add(details, "'stack' must only contain strings");
          break;
        }
      }
    }
  }

  bool is_valid = StringList_is_empty(details);

  if (!is_valid)
    *error = (Error *)create_validation_error(NULL, details);

  StringList_free(details);

  return is_valid;
}

Person *create_person_use_case(DatabaseContext *database_context,
                               cJSON *person_json, Error **error) {

  bool is_person_data_valid =
      validate_person_creation_data(database_context, person_json, error);

  if (!is_person_data_valid)
    return NULL;

  uuid_t id;
  uuid_generate(id);
  char id_string[UUID_STR_LEN] = {'\0'};
  uuid_unparse_lower(id, id_string);

  cJSON_AddStringToObject(person_json, "id", id_string);

  Person *person = person_from_json(person_json);

  Person *saved_person = person_repository_store(database_context, person);

  free_person(person);

  return saved_person;
};

#endif