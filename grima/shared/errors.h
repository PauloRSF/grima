#ifndef GRIMA_SHARED_ERRORS_H
#define GRIMA_SHARED_ERRORS_H

#include <stddef.h>

struct validation_error_entry {
  char *field;
  char **errors;
};

typedef struct hashmap *ValidationErrors;

ValidationErrors ValidationErrors_new();
void ValidationErrors_add(ValidationErrors self, char *field, char *error);
void ValidationErrors_add_many(ValidationErrors self, char *field, char **error, size_t error_count);
size_t ValidationErrors_fields_count(ValidationErrors self);

#ifdef DEV

void ValidationErrors_show(ValidationErrors self);

#endif
#endif
