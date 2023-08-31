#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <hashmap.h>

#include "string_map.h"

int compare(const void *a, const void *b, void *udata) {
  const StringMapEntry *ua = a;
  const StringMapEntry *ub = b;
  return strcmp(ua->key, ub->key);
}

uint64_t hash(const void *item, uint64_t seed0, uint64_t seed1) {
  const StringMapEntry *entry = item;
  return hashmap_sip(entry->key, strlen(entry->key), seed0, seed1);
}

void free_element(void *item) {
  const StringMapEntry *entry = item;
  free(entry->key);
  free(entry->value);
}

StringMap StringMap_new() {
  return hashmap_new(sizeof(StringMapEntry), 1, 0, 0, hash, compare,
                     free_element, NULL);
}

char *StringMap_get(StringMap map, char *key) {
  StringMapEntry *entry =
      (StringMapEntry *)hashmap_get(map, &(StringMapEntry){.key = key});

  if (entry == NULL)
    return NULL;

  return entry->value;
}

void StringMap_set(StringMap map, char *key, char *value) {
  StringMapEntry entry;

  entry.key = calloc(1, strlen(key) + 1);
  strcpy(entry.key, key);

  entry.value = calloc(1, strlen(value) + 1);
  strcpy(entry.value, value);

  hashmap_set(map, &entry);
}

void StringMap_free(StringMap map) { hashmap_free(map); }
