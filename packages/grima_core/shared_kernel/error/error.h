#ifndef GRIMA_SHARED_KERNEL_ERROR_H
#define GRIMA_SHARED_KERNEL_ERROR_H

#include <string_list.h>

typedef enum app_error_kind { ValidationErrorKind } ErrorKind;

typedef struct app_error {
  ErrorKind kind;
} Error;

typedef struct app_validation_error {
  Error base_error;
  char *message;
  StringList *details;
} ValidationError;

ValidationError *create_validation_error(char *message, StringList *details);

void free_error(Error *error);

#endif
