#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <picohttpparser.h>

#include "headers.h"
#include "request.h"

char *get_method_name(Method method) {
  char *methods[] = {"GET", "POST"};

  return methods[method];
}

Method get_method_by_name(char *method_name) {
  if (strcmp(method_name, "POST") == 0) {
    return POST;
  }

  return GET;
}

Request *parse_request(char *raw_request, size_t raw_request_length) {
  struct phr_header parsed_headers[100];
  const char *method = NULL, *path = NULL;
  size_t buffer_length, path_length, method_length,
      num_headers = sizeof(parsed_headers) / sizeof(parsed_headers[0]);
  int minor_version;

  int pret = phr_parse_request(raw_request, raw_request_length, &method,
                               &method_length, &path, &path_length,
                               &minor_version, parsed_headers, &num_headers, 0);

  // if (pret == -1)
  //   return NULL;

  Request *request = calloc(1, sizeof(Request));

  char method_str[method_length + 1];
  memset(method_str, '\0', method_length + 1);
  strncpy(method_str, method, method_length);

  request->method = get_method_by_name(method_str);

  char path_str[path_length + 1];
  memset(path_str, '\0', path_length + 1);
  strncpy(path_str, path, path_length);

  request.path = (char *)calloc(1, path_length + 1);
  strcpy(request.path, path_str);

  Headers headers_map = create_headers();

  char header_key[128] = {'\0'};
  char header_value[128] = {'\0'};

  for (int i = 0; i < num_headers; ++i) {
    memset(header_key, '\0', 128);
    memset(header_value, '\0', 128);

    strncpy(header_key, parsed_headers[i].name, parsed_headers[i].name_len);
    strncpy(header_value, parsed_headers[i].value, parsed_headers[i].value_len);

    add_header(headers_map, header_key, header_value);
  }

  request->headers = headers_map;

  char *raw_request_body = raw_request + pret;

  char *content_length = get_header_value(headers_map, "Content-Length");

  int body_length =
      content_length != NULL ? atoi(content_length) : strlen(raw_request_body);

  request->body = malloc(body_length + 1);
  strcpy(request->body, raw_request_body);

  free(fake_url_to_parse);

  return request;
}

void free_request(Request *request) {
  free(request->path);
  free(request->body);
  free_headers(request->headers);
  free(request);
}
