#ifndef GRIMA_PERSONS_DOMAIN_PERSON_REPOSITORY_H
#define GRIMA_PERSONS_DOMAIN_PERSON_REPOSITORY_H

#include <uuid/uuid.h>

#include "../../../database.h"
#include "person.h"

Person *person_repository_get_by_id(DatabaseContext *database_ctx, uuid_t id);

Person *person_repository_store(DatabaseContext *database_ctx, Person *person);

size_t person_repository_count(DatabaseContext *database_ctx);

#endif