#include <stdlib.h>
#include <string.h>

#include <string_list.h>

#include <grima/error.h>

ValidationError *create_validation_error(char *message, StringList *details) {
  ValidationError *error = calloc(sizeof(ValidationError), 1);

  error->base_error = (Error){.kind = ValidationErrorKind};
  error->details = StringList_clone(details);

  char *error_message =
      message != NULL ? message : "There were validation errors";

  error->message = malloc(strlen(error_message) + 1);
  strcpy(error->message, error_message);

  return error;
}

void free_error(Error *error) {
  if (error == NULL)
    return;

  if (error->kind == ValidationErrorKind) {
    ValidationError *validation_error = (ValidationError *)error;

    free(validation_error->message);
    StringList_free(validation_error->details);
  }
  free(error);
}
