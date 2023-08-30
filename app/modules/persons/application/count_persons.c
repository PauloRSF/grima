#ifndef GRIMA_PERSONS_APPLICATION_GET_PERSON_BY_ID_H
#define GRIMA_PERSONS_APPLICATION_GET_PERSON_BY_ID_H

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

size_t count_persons_use_case(DatabaseContext *database_ctx) {
  return person_repository_count(database_ctx);
};
#endif