#ifndef GRIMA_HEADERS_H
#define GRIMA_HEADERS_H

#include <alloca.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <hashmap.h>

#define HTTP_VERSION "HTTP/1.1"

typedef struct header {
  char *key;
  char *value;
} Header;

typedef struct hashmap *Headers;

int header_compare(const void *a, const void *b, void *udata) {
  const struct header *ua = a;
  const struct header *ub = b;
  return strcmp(ua->key, ub->key);
}

uint64_t header_hash(const void *item, uint64_t seed0, uint64_t seed1) {
  const struct header *header = item;
  return hashmap_sip(header->key, strlen(header->key), seed0, seed1);
}

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

Headers create_headers() {
  return hashmap_new(sizeof(Header), 1, 0, 0, header_hash, header_compare, NULL,
                     NULL);
}

Header *get_header(Headers headers, char *key) {
  return (Header *)hashmap_get(headers, &(Header){.key = key});
}

void add_header(Headers headers, char *key, char *value) {
  Header header;

  header.key = calloc(1, strlen(key) + 1);
  strcpy(header.key, key);

  header.value = calloc(1, strlen(value) + 1);
  strcpy(header.value, value);

  hashmap_set(headers, &header);
}

void free_header(Header *header) {
  free(header->key);
  free(header->value);
}

void free_headers(Headers headers) { hashmap_free(headers); }
#endif
