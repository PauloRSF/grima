#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "headers.h"
#include "request.h"
#include "response.h"
#include "server.h"

void shutdown_server(ServerContext *ctx) {
  close(ctx->client_socket_descriptor);
  shutdown(ctx->server_descriptor, SHUT_RDWR);
};

Request *receive_request(ServerContext *ctx) {
  socklen_t address_size = sizeof(ctx->address);

  int client_socket_descriptor = accept(
      ctx->server_descriptor, (struct sockaddr *)&ctx->address, &address_size);

  if (client_socket_descriptor == -1) {
    perror("Could not open client socket");
    exit(errno);
  }

  ctx->client_socket_descriptor = client_socket_descriptor;
  char *raw_request = NULL;

  char client_receive_buffer[CLIENT_RECEIVE_BUFFER_SIZE] = {0};

  int bytes_read = -1;
  size_t raw_request_length = 0;

  while (true) {
    memset(client_receive_buffer, '\0', CLIENT_RECEIVE_BUFFER_SIZE);

    bytes_read = read(client_socket_descriptor, &client_receive_buffer,
                      CLIENT_RECEIVE_BUFFER_SIZE);

    if (bytes_read == -1) {
      perror("Could not read from client socket");
      exit(errno);
    }

    raw_request_length += bytes_read;

    if (bytes_read != 0) {
      if (raw_request == NULL) {
        raw_request = calloc(1, raw_request_length + 1);
        strncpy(raw_request, client_receive_buffer, bytes_read);
      } else {
        raw_request = realloc(raw_request, raw_request_length + 1);
        strncat(raw_request, client_receive_buffer, bytes_read);
      }
    }

    if (bytes_read == 0 || bytes_read < CLIENT_RECEIVE_BUFFER_SIZE)
      break;
  }

  if (raw_request == NULL)
    return NULL;

  Request *request = parse_request(raw_request, raw_request_length);

  free(raw_request);

  return request;
};

void send_response(ServerContext *ctx, Response *response) {
  char *response_payload = build_http_response_payload(response);

  int write_result = write(ctx->client_socket_descriptor, response_payload,
                           strlen(response_payload));

  if (write_result == -1) {
    perror("Could not write to client socket");
    exit(errno);
  }

  free(response_payload);
};

void accept_connection(ServerContext *ctx, RequestHandler handle_request) {
  Request *request = receive_request(ctx);

  if (request != NULL) {
    Response *response = create_response();

    handle_request(ctx, request, response);

    free_request(request);
    free_response(response);
  }

  close(ctx->client_socket_descriptor);
};

ServerContext init_server(int port) {
  int server_descriptor;

  struct sockaddr_in address = {.sin_family = AF_INET,
                                .sin_addr = {.s_addr = INADDR_ANY},
                                .sin_port = htons(port)};

  socklen_t address_size = sizeof(address);

  server_descriptor = socket(AF_INET, SOCK_STREAM, 0);

  if (server_descriptor == -1) {
    perror("Could not open server socket");
    exit(errno);
  }

  int bind_result =
      bind(server_descriptor, (struct sockaddr *)&address, address_size);

  if (bind_result == -1) {
    perror("Could not bind server socket");
    exit(errno);
  }

  int listen_result = listen(server_descriptor, 1);

  if (listen_result == -1) {
    perror("Could not listen in server socket");
    exit(errno);
  }

  ServerContext ctx = {.server_descriptor = server_descriptor};

  return ctx;
};

void start_server(ServerContext *ctx, RequestHandler handle_request) {
  while (true) {
    accept_connection(ctx, handle_request);
  }
}