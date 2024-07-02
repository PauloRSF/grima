#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <hashmap.h>

#include "headers.h"
#include "string_map.h"

char *get_query_string(StringMap query_params) {
  size_t index = 0, items_count = 0;

  unsigned int query_string_length = 0;
  void *item;

  while (hashmap_iter(query_params, &index, &item)) {
    const Header *header = item;
    query_string_length += strlen(header->key) + strlen(header->value) + 4;
    ++items_count;
  }

  char *query_string = calloc(query_string_length + 1, 1);

  index = 0;

  while (hashmap_iter(query_params, &index, &item)) {
    const Header *header = item;
    strcat(query_string, header->key);
    strcat(query_string, "=");
    strcat(query_string, header->value);

    if (--items_count)
      strcat(query_string, "&");
  }

  return query_string;
}

char *get_query_param_value(StringMap query_params, char *key) {
  return StringMap_get(query_params, key);
}
