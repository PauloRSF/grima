#ifndef GRIMA_PERSONS_DOMAIN_PERSON_H
#define GRIMA_PERSONS_DOMAIN_PERSON_H

#include <cJSON.h>
#include <string_list.h>
#include <uuid/uuid.h>

#include "../../../shared_kernel/dates/date.h"

typedef struct person {
  uuid_t id;
  char *name;
  char *nickname;
  Date date_of_birth;
  StringList *stack;
} Person;

typedef struct person_search_result {
  Person **persons;
  size_t persons_count;
} PersonSearchResult;

StringList *copy_person_stack(StringList *stack);

Person *create_person(uuid_t id, char *name, char *nickname, Date date_of_birth,
                      StringList *stack);

char *person_to_json(Person *person);

Person *person_from_json(cJSON *person_json);

char *persons_to_json(Person **person, size_t person_count);

void free_person(Person *person);

void free_person_search_result(PersonSearchResult *result);

#endif