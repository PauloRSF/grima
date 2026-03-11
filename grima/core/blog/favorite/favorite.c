#include <stdbool.h>
#include <stdlib.h>

#include <lib/id.h>
#include <shared/errors.h>

#include "favorite.h"

ValidationErrors validate_favorite(struct favorite *favorite) {
  ValidationErrors validation_errors = ValidationErrors_new();

  char *author_id_validation_error = validate_entity_id(favorite->author_id);

  if (author_id_validation_error != NULL) {
    ValidationErrors_add(validation_errors, "author_id", author_id_validation_error);
  }

  char *article_id_validation_error = validate_entity_id(favorite->article_id);

  if (article_id_validation_error != NULL) {
    ValidationErrors_add(validation_errors, "article_id", article_id_validation_error);
  }

  if (ValidationErrors_fields_count(validation_errors) == 0) {
    ValidationErrors_free(validation_errors);
    return NULL;
  }

  return validation_errors;
}

struct create_favorite_result Favorite_create(entity_id_t author_id, entity_id_t article_id) {
  struct create_favorite_result result;

  struct favorite *favorite = malloc(sizeof(struct favorite));

  favorite->author_id = author_id;
  favorite->article_id = article_id;

  ValidationErrors validation_errors = validate_favorite(favorite);

  if (validation_errors != NULL) {
    free(favorite);

    result.success = false;
    result.value.error = validation_errors;

    return result;
  }

  ValidationErrors_free(validation_errors);

  epoch_ms_t now = current_unix_timestamp();
  favorite->created_at = now;
  favorite->id = FavoriteRepository_get_next_id();

  result.success = true;
  result.value.success = favorite;

  return result;
}

void Favorite_free(struct favorite *favorite) {
  if (favorite->id != NULL)
    free_entity_id(favorite->id);

  if (favorite->author_id != NULL)
    free(favorite->author_id);

  if (favorite->article_id != NULL)
    free(favorite->article_id);

  free(favorite);
}

#ifdef DEV
#include <stdio.h>

void Favorite_show(struct favorite *favorite) {
  char *created_at = unix_timestamp_to_iso8601(favorite->created_at);

  printf("Favorite {\n\t"
         "id = \"%s\",\n\t"
         "author_id = \"%s\",\n\t"
         "article_id = \"%s\",\n\t"
         "created_at = \"%s\"\n"
         "}\n",
         favorite->id, favorite->author_id, favorite->article_id, created_at);

  free(created_at);
}

void Favorite_show_creation_result(struct create_favorite_result result) {
  printf("Favorite creation result: %s\n", result.success ? "success" : "failure");

  if (result.success) {
    Favorite_show(result.value.success);
  } else {
    ValidationErrors_show(result.value.error);
  }
}

#endif
