#ifndef GRIMA_SERVER_H
#define GRIMA_SERVER_H
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "headers.c"
#include "request.c"
#include "response.c"

#define CLIENT_RECEIVE_BUFFER_SIZE 1024

typedef struct server_context {
  int server_descriptor;
  struct sockaddr_in address;
  int client_socket_descriptor;
} ServerContext;

ServerContext start_server(int port) {
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

void shutdown_server(ServerContext ctx) {
  close(ctx.client_socket_descriptor);
  shutdown(ctx.server_descriptor, SHUT_RDWR);
};

Request receive_request(ServerContext *ctx) {
  socklen_t address_size = sizeof(ctx->address);

  int client_socket_descriptor = accept(
      ctx->server_descriptor, (struct sockaddr *)&ctx->address, &address_size);

  if (client_socket_descriptor == -1) {
    perror("Could not open client socket");
    exit(errno);
  }

  ctx->client_socket_descriptor = client_socket_descriptor;

  char client_receive_buffer[CLIENT_RECEIVE_BUFFER_SIZE] = {0};

  int read_result = read(client_socket_descriptor, &client_receive_buffer,
                         CLIENT_RECEIVE_BUFFER_SIZE);

  if (read_result == -1) {
    perror("Could not read from client socket");
    exit(errno);
  }

  Request request = parse_request(client_receive_buffer);

  return request;
};

void send_response(ServerContext *ctx, Response response) {
  char *response_payload = build_http_response_payload(response);

  int write_result = write(ctx->client_socket_descriptor, response_payload,
                           strlen(response_payload));

  if (write_result == -1) {
    perror("Could not write to client socket");
    exit(errno);
  }

  free(response_payload);
};

void accept_connection(ServerContext *ctx,
                       void (*handle_request)(ServerContext *ctx,
                                              Request request,
                                              Response response)) {
  Request request = receive_request(ctx);

  Response response = create_response();

  handle_request(ctx, request, response);

  free_request(&request);
  free_response(&response);

  close(ctx->client_socket_descriptor);
};

#endif
