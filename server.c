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
#define PORT 3000

struct server_context {
  int server_descriptor;
  struct sockaddr_in address;
  int client_socket_descriptor;
};

struct server_context start_server(int port) {
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

  struct server_context ctx = {.server_descriptor = server_descriptor};

  return ctx;
};

void shutdown_server(struct server_context ctx) {
  shutdown(ctx.server_descriptor, SHUT_RDWR);
};

Request receive_request(struct server_context *ctx) {
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

void send_response(struct server_context ctx, unsigned short status_code,
                   Headers headers, char *body) {
  char *response = build_http_response_payload(status_code, headers, body);

  int write_result =
      write(ctx.client_socket_descriptor, response, strlen(response));

  if (write_result == -1) {
    perror("Could not write to client socket");
    exit(errno);
  }

  free(response);
};
