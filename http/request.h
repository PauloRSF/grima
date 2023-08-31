#ifndef GRIMA_REQUEST_H
#define GRIMA_REQUEST_H

#include <picohttpparser.h>

typedef enum method {
  GET = 0,
  POST = 1,
} Method;

typedef struct request {
  Method method;
  char *path;
  struct hashmap *headers;
  char *body;
} Request;

char *get_method_name(Method method);

Method get_method_by_name(char *method_name);

Request *parse_request(char *raw_request, size_t raw_request_length);

void free_request(Request *request);
#endif
