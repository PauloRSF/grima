#ifndef GRIMA_PERSONS_APPLICATION_GET_PERSON_BY_ID_H
#define GRIMA_PERSONS_APPLICATION_GET_PERSON_BY_ID_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cJSON.h>
#include <libpq-fe.h>
#include <uuid/uuid.h>

#include "../../../database.h"
#include "../../../shared/dates/date.h"
#include "../domain/person.h"
#include "../domain/person_repository.h"

Person *get_person_by_id_use_case(DatabaseContext *database_ctx,
                                  uuid_t person_id) {
  return person_repository_get_by_id(database_ctx, person_id);
};
#endif