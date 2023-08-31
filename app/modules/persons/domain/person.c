#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <string_list.h>
#include <uuid/uuid.h>

#include "../../../shared_kernel/dates/date.h"
#include "person.h"

Person *create_person(uuid_t id, char *name, char *nickname, Date date_of_birth,
                      StringList *stack) {
  Person *person = (Person *)calloc(1, sizeof(Person));

  // HACK: how to replace this memcpy for uuid_copy?
  if (id != NULL) {
    memcpy(person->id, id, sizeof(uuid_t));
  } else {
    uuid_t new_id;
    uuid_generate(new_id);
    memcpy(person->id, new_id, sizeof(uuid_t));
  }

  person->name = calloc(1, strlen(name) + 1);
  strcpy(person->name, name);

  person->nickname = calloc(1, strlen(nickname) + 1);
  strcpy(person->nickname, nickname);

  person->date_of_birth = date_of_birth;
  person->stack = stack != NULL ? StringList_clone(stack) : NULL;

  return person;
};

void free_person(Person *person) {
  if (person == NULL)
    return;

  if (person->name)
    free(person->name);

  if (person->nickname)
    free(person->nickname);

  if (person->stack) {
    StringList_free(person->stack);
  }

  free(person);
}
