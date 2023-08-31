#include <stdlib.h>
#include <string.h>

#include <string_list.h>

#include "error.h"

ValidationError *create_validation_error(char *message, StringList *details) {
  ValidationError *error = calloc(sizeof(ValidationError), 1);

  error->base_error = (Error){.kind = ValidationErrorKind};
  error->details = StringList_clone(details);
  error->message = NULL;

  if (message != NULL) {
    error->message = malloc(strlen(message) + 1);
    strcpy(error->message, message);
  } else {
    error->message = "There were validation errors";
  }

  return error;
}

void free_error(Error *error) {
  if (error->kind == ValidationErrorKind) {
    ValidationError *validation_error = (ValidationError *)&error;

    // free(validation_error->message);
    StringList_free(validation_error->details);
  }
  free(error);
}
