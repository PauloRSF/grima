#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cJSON.h>
#include <uuid/uuid.h>

#include "../../../shared_kernel/dates/date.h"
#include "../domain/person.h"

char *person_to_json(Person *person) {
  cJSON *json = cJSON_CreateObject();

  char person_id_string[37];
  uuid_unparse_lower(person->id, person_id_string);

  if (cJSON_AddStringToObject(json, "id", person_id_string) == NULL) {
    cJSON_free(json);
    return NULL;
  };

  if (cJSON_AddStringToObject(json, "nome", person->name) == NULL) {
    cJSON_free(json);
    return NULL;
  };

  if (cJSON_AddStringToObject(json, "apelido", person->nickname) == NULL) {
    cJSON_free(json);
    return NULL;
  };

  char *date_of_birth_string = date_to_string(person->date_of_birth);

  if (cJSON_AddStringToObject(json, "nascimento", date_of_birth_string) ==
      NULL) {
    free(date_of_birth_string);
    cJSON_free(json);
    return NULL;
  };

  free(date_of_birth_string);

  if (person->stack != NULL) {
    cJSON *stack = cJSON_AddArrayToObject(json, "stack");

    for (int i = 0; person->stack[i] != NULL; ++i) {
      cJSON *tech = cJSON_CreateString(person->stack[i]);
      cJSON_AddItemToArray(stack, tech);
    }
  } else {
    cJSON_AddNullToObject(json, "stack");
  }

  char *json_person = cJSON_Print(json);

  cJSON_free(json);

  return json_person;
}

size_t count_person_json_stack_items(cJSON *stack_json) {
  int i = 0;
  cJSON *item;
  size_t count = 0;

  cJSON_ArrayForEach(item, stack_json) { ++count; }

  cJSON_free(item);

  return count;
}

char **copy_person_json_stack_items(cJSON *stack_json) {
  size_t i = 0;
  cJSON *stack_item;
  size_t stack_items_count = count_person_json_stack_items(stack_json);
  char **stack = calloc(sizeof(char *), stack_items_count);

  cJSON_ArrayForEach(stack_item, stack_json) {
    if (cJSON_IsString(stack_item) && stack_item->valuestring != NULL) {
      stack[i] = malloc(strlen(stack_item->valuestring) + 1);

      strcpy(stack[i], stack_item->valuestring);

      ++i;
    }
  }

  cJSON_free(stack_item);

  return stack;
}

Person *person_from_json(cJSON *person_json) {
  uuid_t id = {'\0'};
  char *name = NULL, *nickname = NULL, **stack = NULL;
  Date date_of_birth;

  cJSON *id_json = cJSON_GetObjectItemCaseSensitive(person_json, "id");

  if (cJSON_IsString(id_json) && id_json->valuestring) {
    uuid_parse(id_json->valuestring, id);
  }

  cJSON *name_json = cJSON_GetObjectItemCaseSensitive(person_json, "nome");

  if (cJSON_IsString(name_json) && name_json->valuestring) {
    name = name_json->valuestring;
  }

  cJSON *nickname_json =
      cJSON_GetObjectItemCaseSensitive(person_json, "apelido");

  if (cJSON_IsString(nickname_json) && nickname_json->valuestring) {
    nickname = nickname_json->valuestring;
  }

  cJSON *date_of_birth_json =
      cJSON_GetObjectItemCaseSensitive(person_json, "nascimento");

  if (cJSON_IsString(date_of_birth_json) && date_of_birth_json->valuestring) {
    date_of_birth = date_from_string(date_of_birth_json->valuestring);
  }

  cJSON *stack_json = cJSON_GetObjectItemCaseSensitive(person_json, "stack");

  stack = copy_person_json_stack_items(stack_json);

  Person *person = create_person(id, name, nickname, date_of_birth, stack);

  free(stack);

  return person;
}
