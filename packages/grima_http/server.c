#include <errno.h>
#include <liburing.h>
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
  io_uring_queue_exit(ctx->ring);
  shutdown(ctx->server_descriptor, SHUT_RDWR);
};

struct io_uring ring;

char client_buffers[65536][CLIENT_RECEIVE_BUFFER_SIZE];

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

  ctx.ring = &ring;

  io_uring_queue_init(256, ctx.ring, 0);

  return ctx;
};

void request_accept_connection(ServerContext *ctx) {
  struct io_uring_sqe *sqe = io_uring_get_sqe(ctx->ring);

  io_uring_prep_accept(sqe, ctx->server_descriptor, NULL, NULL, 0);

  struct iorequest *req = malloc(sizeof(struct iorequest *));

  req->event_type = EVENT_TYPE_ACCEPT_CONNECTION;

  io_uring_sqe_set_data(sqe, req);

  io_uring_submit(ctx->ring);
}

void request_read_request(ServerContext *ctx, int client_descriptor) {
  struct io_uring_sqe *sqe = io_uring_get_sqe(ctx->ring);

  io_uring_prep_read(sqe, client_descriptor, client_buffers[client_descriptor],
                     CLIENT_RECEIVE_BUFFER_SIZE, 0);

  struct iorequest *req = malloc(sizeof(struct iorequest *));

  req->event_type = EVENT_TYPE_HANDLE_REQUEST;
  req->client_socket = client_descriptor;

  io_uring_sqe_set_data(sqe, req);

  io_uring_submit(ctx->ring);
}

void send_response(ServerContext *server_ctx, RequestContext *request_ctx,
                   Response *response) {
  struct io_uring_sqe *sqe = io_uring_get_sqe(server_ctx->ring);

  char *response_payload = build_http_response_payload(response);

  io_uring_prep_write(sqe, request_ctx->client_socket_descriptor,
                      response_payload, strlen(response_payload), 0);

  struct iorequest *req = malloc(sizeof(struct iorequest *));

  req->event_type = EVENT_TYPE_CLOSE_CONNECTION;
  req->client_socket = request_ctx->client_socket_descriptor;

  io_uring_sqe_set_data(sqe, req);

  io_uring_submit(server_ctx->ring);

  free(response_payload);
};

void start_server(ServerContext *ctx, RequestHandler handle_request) {
  struct io_uring_cqe *cqe;

  request_accept_connection(ctx);

  while (1) {
    int ret = io_uring_wait_cqe(ctx->ring, &cqe);

    struct iorequest *req = (struct iorequest *)cqe->user_data;

    if (ret < 0) {
      perror("io_uring_wait_cqe");
      exit(1);
    }

    if (cqe->res < 0) {
      fprintf(stderr, "Async request failed: %s for event: %d\n",
              strerror(-cqe->res), req->event_type);
    }

    switch (req->event_type) {
    case EVENT_TYPE_ACCEPT_CONNECTION:
      request_accept_connection(ctx);

      request_read_request(ctx, cqe->res);

      free(req);

      break;
    case EVENT_TYPE_HANDLE_REQUEST:
      if (cqe->res > 0) {
        Request *request =
            parse_request(client_buffers[req->client_socket], cqe->res);

        Response *response = create_response();

        handle_request(
            &(ServerContext){.server_descriptor = ctx->server_descriptor,
                             .client_socket_descriptor = req->client_socket,
                             .ring = ctx->ring,
                             .address = ctx->address},
            request, response);

        free_request(request);
        free_response(response);
      }

      free(req);

      break;
    case EVENT_TYPE_CLOSE_CONNECTION:
      close(req->client_socket);

      free(req);

      break;
    }

    io_uring_cqe_seen(ctx->ring, cqe);
  }
}
