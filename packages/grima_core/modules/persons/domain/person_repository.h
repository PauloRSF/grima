#ifndef GRIMA_CORE_PERSONS_DOMAIN_PERSON_REPOSITORY_H
#define GRIMA_CORE_PERSONS_DOMAIN_PERSON_REPOSITORY_H

#include <uuid/uuid.h>

#include "../../../database.h"
#include "person.h"

Person *person_repository_get_by_id(DatabaseContext *database_ctx, uuid_t id);

bool person_repository_is_nickname_taken(DatabaseContext *database_ctx,
                                         char *nickname);

Person *person_repository_store(DatabaseContext *database_ctx, Person *person);

size_t person_repository_count(DatabaseContext *database_ctx);

PersonSearchResult person_repository_search(DatabaseContext *database_ctx,
                                            char *search_term);

#endif