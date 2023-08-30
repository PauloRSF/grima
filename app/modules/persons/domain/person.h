#ifndef GRIMA_PERSONS_DOMAIN_PERSON_H
#define GRIMA_PERSONS_DOMAIN_PERSON_H

#include <cJSON.h>
#include <uuid/uuid.h>

#include "../../dates/domain/date.h"

typedef struct person {
  uuid_t id;
  char *name;
  char *nickname;
  Date date_of_birth;
  char **stack;
} Person;

char **copy_person_stack(char **stack);

Person *create_person(uuid_t id, char *name, char *nickname, Date date_of_birth,
                      char **stack);

char *person_to_json(Person *person);

Person *person_from_json(cJSON *person_json);

void free_person(Person *person);
#endif