#ifndef GRIMA_RESPONSE_H
#define GRIMA_RESPONSE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "headers.c"

#define HTTP_VERSION "HTTP/1.1"

#define HTTP_OK_TEXT "OK"
#define HTTP_NOT_FOUND_TEXT "Not Found"

char *get_status_line(unsigned short status_code) {
  char *status_text;

  switch (status_code) {
  case 200:
    status_text = (char *)calloc(1, strlen(HTTP_OK_TEXT));
    sprintf(status_text, HTTP_OK_TEXT);
    break;
  case 404:
    status_text = (char *)calloc(1, strlen(HTTP_NOT_FOUND_TEXT));
    sprintf(status_text, HTTP_NOT_FOUND_TEXT);
    break;
  }

  char *status_line =
      (char *)calloc(1, strlen(HTTP_VERSION) + 5 + strlen(status_text));

  sprintf(status_line, "%s %d %s\r\n", HTTP_VERSION, status_code, status_text);

  free(status_text);

  return status_line;
}

char *build_http_response_payload(unsigned short status_code, Headers headers,
                                  char *body) {
  char *response_payload;

  if (body != NULL) {
    char body_length_str[32];
    sprintf(body_length_str, "%lu", strlen(body) + 4);
    add_header(headers, "Content-Length", body_length_str);
  }

  char *status_line = get_status_line(status_code);
  char *headers_payload = get_headers_payload(headers);

  size_t response_payload_length = strlen(status_line) + 2;

  if (headers != NULL)
    response_payload_length += strlen(headers_payload);

  if (body != NULL)
    response_payload_length += strlen(body);

  response_payload = (char *)calloc(1, response_payload_length + 1);

  strcpy(response_payload, status_line);

  if (headers != NULL)
    strcat(response_payload, headers_payload);

  if (body != NULL) {
    strcat(response_payload, "\r\n");
    strcat(response_payload, body);
    strcat(response_payload, "\r\n");
  }

  strcat(response_payload, "\r\n");

  free(headers_payload);
  free(status_line);

  return response_payload;
}
#endif
