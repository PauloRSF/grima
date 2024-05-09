#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <hashmap.h>
#include <string_map.h>

#include "headers.h"

char *get_headers_payload(Headers headers) {
  size_t index = 0;

  unsigned int headers_length = 0;
  void *item;

  while (hashmap_iter(headers, &index, &item)) {
    const Header *header = item;
    headers_length += strlen(header->key) + strlen(header->value) + 4;
  }

  char *headers_payload = (char *)calloc(1, headers_length + 1);

  index = 0;

  while (hashmap_iter(headers, &index, &item)) {
    const Header *header = item;
    strcat(headers_payload, header->key);
    strcat(headers_payload, ": ");
    strcat(headers_payload, header->value);
    strcat(headers_payload, "\r\n");
  }

  return headers_payload;
}

Headers create_headers() { return StringMap_new(); }

char *get_header_value(Headers headers, char *key) {
  return StringMap_get(headers, key);
}

void add_header(Headers headers, char *key, char *value) {
  return StringMap_set(headers, key, value);
}

bool headers_iter(Headers headers, size_t *i, void **item) {
  return StringMap_iter(headers, i, item);
}

void free_headers(Headers headers) { StringMap_free(headers); }
