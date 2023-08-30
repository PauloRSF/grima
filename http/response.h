#ifndef GRIMA_RESPONSE_H
#define GRIMA_RESPONSE_H

#include "headers.h"

#define HTTP_VERSION "HTTP/1.1"

#define HTTP_OK_TEXT "OK"
#define HTTP_CREATED_TEXT "Created"
#define HTTP_BAD_REQUEST_TEXT "Bad Request"
#define HTTP_NOT_FOUND_TEXT "Not Found"
#define HTTP_METHOD_NOT_ALLOWED_TEXT "Method Not Allowed"
#define HTTP_UNSUPPORTED_MEDIA_TYPE_TEXT "Unsupported Media Type"
#define HTTP_UNPROCESSABLE_ENTITY_TEXT "Unprocessable Entity"
#define HTTP_INTERNAL_SERVER_ERROR_TEXT "Internal Server Error"

#define HTTP_METHOD_LENGTH 3
#define CRLF_LENGTH 2

typedef struct response {
  unsigned short status_code;
  Headers headers;
  char *body;
} Response;

Response create_response();

char *get_status_line(unsigned short status_code);

char *build_http_response_payload(Response response);

void free_response(Response *response);
#endif
