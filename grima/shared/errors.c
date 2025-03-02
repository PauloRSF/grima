#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <hashmap.h>

#include "errors.h"

int compare_validation_error(const void *a, const void *b, void *udata) {
  const struct validation_error_entry *ua = a;
  const struct validation_error_entry *ub = b;

  return strcmp(ua->field, ub->field);
}

uint64_t hash_validation_error(const void *item, uint64_t seed0, uint64_t seed1) {
  const struct validation_error_entry *entry = item;

  return hashmap_sip(entry->field, strlen(entry->field), seed0, seed1);
}

ValidationErrors ValidationErrors_new() {
  return hashmap_new(sizeof(struct validation_error_entry), 1, 0, 0, hash_validation_error, compare_validation_error,
                     NULL, NULL);
}

void ValidationErrors_add(ValidationErrors self, char *field, char *error) {
  const struct validation_error_entry *entry = hashmap_get(self, &(struct validation_error_entry){.field = field});

  if (entry == NULL) {
    char **errors = malloc(2 * sizeof(char *));
    errors[0] = error;
    errors[1] = NULL;

    struct validation_error_entry new_entry = {
        .field = field,
        .errors = errors,
    };

    hashmap_set(self, &new_entry);

    return;
  }

  struct validation_error_entry *updated_entry = malloc(sizeof(struct validation_error_entry));

  updated_entry->field = entry->field;

  size_t error_count = 0;
  char **errors = entry->errors;

  while (errors[error_count] != NULL) {
    error_count++;
  }

  errors = realloc(errors, sizeof(char *) * (error_count + 2));
  errors[error_count] = error;
  errors[error_count + 1] = NULL;

  updated_entry->errors = errors;

  hashmap_set(self, updated_entry);
}

void ValidationErrors_add_many(ValidationErrors self, char *field, char **error, size_t error_count) {
  for (size_t i = 0; i < error_count; i++) {
    ValidationErrors_add(self, field, error[i]);
  }
}

size_t ValidationErrors_fields_count(ValidationErrors self) { return hashmap_count(self); }

#ifdef DEV
#include <stdio.h>

void ValidationErrors_show(ValidationErrors self) {
  void *item;
  size_t i = 0;

  while (hashmap_iter(self, &i, &item)) {
    struct validation_error_entry *entry = item;

    printf("[.%s]\n", entry->field);

    char **errors = entry->errors;

    for (size_t j = 0; errors[j]; j++) {
      printf("  - %s\n", errors[j]);
    }
  }
}

#endif
