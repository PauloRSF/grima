#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <uuid/uuid.h>

#include "../../dates/domain/date.h"
#include "person.h"

char **copy_person_stack(char **stack) {
  if (stack == NULL)
    return NULL;

  size_t tech_count = 0;

  while (stack[tech_count++])
    ;

  char **person_stack = calloc(sizeof(char *), tech_count + 1);

  int i = 0;

  for (int i = 0; stack[i]; ++i) {
    person_stack[i] = malloc(strlen(stack[i]) + 1);
    strcpy(person_stack[i], stack[i]);
  }

  return person_stack;
}

Person *create_person(uuid_t id, char *name, char *nickname, Date date_of_birth,
                      char **stack) {
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
  person->stack = stack != NULL ? copy_person_stack(stack) : NULL;

  return person;
};

void free_person(Person *person) {
  if (person->name)
    free(person->name);

  if (person->nickname)
    free(person->nickname);

  if (person->stack)
    for (int i = 0; person->stack[i]; ++i)
      free(person->stack[i]);
}