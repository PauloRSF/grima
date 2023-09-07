#ifndef GRIMA_PERSONS_USE_CASES_H
#define GRIMA_PERSONS_USE_CASES_H

#include "../../../database.h"
#include "../domain/person.h"
#include "shared_kernel/shared_kernel.h"

Person *get_person_by_id_use_case(DatabaseContext *database_ctx,
                                  uuid_t person_id);

PersonSearchResult find_persons_use_case(DatabaseContext *database_ctx,
                                         char *search_term);

Person *create_person_use_case(DatabaseContext *database_context,
                               cJSON *person_json, Error **error);

size_t count_persons_use_case(DatabaseContext *database_ctx);

#endif