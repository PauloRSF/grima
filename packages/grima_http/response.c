#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "headers.h"
#include "response.h"

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

Response *create_response() {
  Response *response = calloc(1, sizeof(Response));

  response->status_code = 200;
  response->headers = create_headers();
  response->body = NULL;

  return response;
}

const char *get_status_text(unsigned short status_code) {
  switch (status_code) {
  case 200:
    return HTTP_OK_TEXT;
    break;
  case 201:
    return HTTP_CREATED_TEXT;
    break;
  case 400:
    return HTTP_BAD_REQUEST_TEXT;
    break;
  case 404:
    return HTTP_NOT_FOUND_TEXT;
    break;
  case 405:
    return HTTP_METHOD_NOT_ALLOWED_TEXT;
    break;
  case 415:
    return HTTP_UNSUPPORTED_MEDIA_TYPE_TEXT;
    break;
  case 422:
    return HTTP_UNPROCESSABLE_ENTITY_TEXT;
    break;
  case 500:
    return HTTP_INTERNAL_SERVER_ERROR_TEXT;
    break;
  default:
    return "Unknown";
    break;
  }
}

char *get_status_line(unsigned short status_code) {
  const char *status_text = get_status_text(status_code);

  char *status_line =
      (char *)calloc(1, strlen(HTTP_VERSION) + strlen(status_text) +
                            HTTP_METHOD_LENGTH + CRLF_LENGTH + 4);

  sprintf(status_line, "%s %d %s\r\n", HTTP_VERSION, status_code, status_text);

  return status_line;
}

char *build_http_response_payload(Response *response) {
  char *response_payload;

  if (response->body != NULL) {
    char body_length_str[32];
    sprintf(body_length_str, "%lu", strlen(response->body) + 4);
    add_header(response->headers, "Content-Length", body_length_str);
  } else {
    add_header(response->headers, "Content-Length", "0");
  }

  char *status_line = get_status_line(response->status_code);
  char *headers_payload = get_headers_payload(response->headers);

  size_t response_payload_length = strlen(status_line) + 2;

  if (response->headers != NULL)
    response_payload_length += strlen(headers_payload);

  if (response->body != NULL)
    response_payload_length += strlen(response->body) + CRLF_LENGTH * 2;

  response_payload = (char *)calloc(1, response_payload_length + 10);

  strcpy(response_payload, status_line);

  if (response->headers != NULL)
    strcat(response_payload, headers_payload);

  if (response->body != NULL) {
    strcat(response_payload, "\r\n");
    strcat(response_payload, response->body);
    strcat(response_payload, "\r\n");
  }

  strcat(response_payload, "\r\n");

  free(headers_payload);
  free(status_line);

  return response_payload;
}

void free_response(Response *response) {
  // free(response->body);
  free_headers(response->headers);
  free(response);
}
