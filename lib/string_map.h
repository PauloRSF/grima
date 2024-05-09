#ifndef STRING_MAP_H
#define STRING_MAP_H

#include <hashmap.h>

#define HTTP_VERSION "HTTP/1.1"

typedef struct string_map_entry {
  char *key;
  char *value;
} StringMapEntry;

typedef struct hashmap *StringMap;

StringMap StringMap_new();

char *StringMap_get(StringMap string_map, char *key);

void StringMap_set(StringMap string_map, char *key, char *value);

bool StringMap_iter(StringMap string_map, size_t *i, void **item);

void StringMap_free(StringMap string_map);

#endif
