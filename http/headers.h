#ifndef GRIMA_HEADERS_H
#define GRIMA_HEADERS_H

#include <hashmap.h>

#define HTTP_VERSION "HTTP/1.1"

typedef struct header {
  char *key;
  char *value;
} Header;

typedef struct hashmap *Headers;

char *get_headers_payload(Headers headers);

Headers create_headers();

void add_header(Headers headers, char *key, char *value);

char *get_header_value(Headers headers, char *key);

void free_header(Header *header);

void free_headers(Headers headers);
#endif
