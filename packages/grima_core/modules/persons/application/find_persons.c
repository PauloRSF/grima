#ifndef GRIMA_PERSONS_APPLICATION_FIND_PERSONS_H
#define GRIMA_PERSONS_APPLICATION_FIND_PERSONS_H

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

PersonSearchResult find_persons_use_case(DatabaseContext *database_ctx,
                                         char *search_term) {
  return person_repository_search(database_ctx, search_term);
};

#endif