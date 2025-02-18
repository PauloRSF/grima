#include <errno.h>
#include <liburing.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "headers.h"
#include "request.h"
#include "response.h"
#include "server.h"

void shutdown_server(WormtongueServerContext *ctx) {
  shutdown(ctx->server_descriptor, SHUT_RDWR);
};

char client_buffers[65536][CLIENT_RECEIVE_BUFFER_SIZE];

WormtongueServerContext setup_server(int port, wormtongue_log_fn log) {
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

  WormtongueServerContext ctx = {.server_descriptor = server_descriptor,
                                 .log = log};

  if (ctx.log == NULL)
    ctx.log = printf;

  return ctx;
};

void send_response(Request *request, Response *response) {
  char *response_payload = build_http_response_payload(response);

  write(request->remote_socket, response_payload, strlen(response_payload));

  free(response_payload);
};

pthread_t threads[16];

struct worker_thread_args {
  int client_socket;
  WormtongueServerContext *server_ctx;
  RequestHandler handle_request;
};

#define NUM 1

pthread_cond_t cvs[NUM];
pthread_mutex_t mps[NUM];

struct worker_thread_args *args[NUM] = {NULL};

struct worker_thread_init_data {
  int id;
};

void *handle_request_worker(void *arg) {
  int id = ((struct worker_thread_init_data *)arg)->id;

  printf("Thread %d initialized\n", id);

  while (true) {
    pthread_cond_wait(&cvs[id], &mps[id]);

    printf("Serving client in socket %d in thread %d\n",
           args[id]->client_socket, id);

    int bytes_read =
        read(args[id]->client_socket, client_buffers[args[id]->client_socket],
             CLIENT_RECEIVE_BUFFER_SIZE);

    Request *request =
        parse_request(client_buffers[args[id]->client_socket], bytes_read);

    // Bad request if == NULL
    if (request != NULL) {
      request->remote_socket = args[id]->client_socket;

      Response *response = create_response();

      args[id]->handle_request(request, response);

      free_request(request);
      free_response(response);
    }

    close(args[id]->client_socket);
    args[id] = NULL;
  }

  return NULL;
}

struct worker_thread_args zxc[NUM];

void initialize_worker_threads(WormtongueServerContext *ctx) {
  for (int i = 0; i < NUM; ++i) {
    ctx->log("Initializing thread %d\n", i);

    pthread_cond_init(&cvs[i], NULL);

    struct worker_thread_init_data *init_data =
        malloc(sizeof(struct worker_thread_init_data));
    init_data->id = i;

    pthread_create(&threads[i], NULL, handle_request_worker, init_data);
  }

  ctx->log("%d threads initialized\n", NUM);
}

size_t serving_thread_index = 0;

void wait_for_new_connections(WormtongueServerContext *ctx,
                              RequestHandler handle_request) {
  int client_socket = accept(ctx->server_descriptor, NULL, NULL);

  // TODO: handle this properly
  if (client_socket == -1) {
    printf("Could not accept client connection %d %d %d\n", errno,
           ctx->server_descriptor, client_socket);
    perror("Could not accept client connection");
    return;
  }

  ctx->log("Accepted client connection in socket %d\n", client_socket);

  while (args[serving_thread_index] != NULL)
    serving_thread_index = (serving_thread_index + 1) % NUM;

  zxc[serving_thread_index].client_socket = client_socket;
  zxc[serving_thread_index].server_ctx = ctx;
  zxc[serving_thread_index].handle_request = handle_request;

  args[serving_thread_index] = &zxc[serving_thread_index];

  pthread_cond_signal(&cvs[serving_thread_index]);
}

struct connection_worker_args {
  WormtongueServerContext *server_ctx;
  RequestHandler handle_request;
};

void *handle_connection_worker(void *arg) {
  struct connection_worker_args *args = (struct connection_worker_args *)arg;

  size_t serving_thread_index = 0;

  args->server_ctx->log("Connection thread initialized\n");

  while (true)
    wait_for_new_connections(args->server_ctx, args->handle_request);

  return NULL;
}

void initialize_connection_thread(WormtongueServerContext *ctx,
                                  RequestHandler handle_request) {
  pthread_t connection_thread;

  struct connection_worker_args *args =
      malloc(sizeof(struct connection_worker_args));
  args->server_ctx = ctx;
  args->handle_request = handle_request;

  pthread_create(&connection_thread, NULL, handle_connection_worker, args);
}

void start_server(WormtongueServerContext *ctx, RequestHandler handle_request,
                  bool blocking) {
  initialize_worker_threads(ctx);

  if (!blocking)
    return initialize_connection_thread(ctx, handle_request);

  while (true)
    wait_for_new_connections(ctx, handle_request);
}
