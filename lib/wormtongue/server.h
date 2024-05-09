#ifndef WORMTONGUE_SERVER_H
#define WORMTONGUE_SERVER_H

#include <liburing.h>
#include <netinet/in.h>

#include "headers.h"
#include "request.h"
#include "response.h"

#define CLIENT_RECEIVE_BUFFER_SIZE 2048

typedef struct server_context {
  int server_descriptor;
  struct io_uring *ring;
} WormtongueServerContext;

struct iorequest {
  int event_type;
  int client_socket;
  pthread_t thread;
};

#define EVENT_TYPE_ACCEPT_CONNECTION 0
#define EVENT_TYPE_CLOSE_CONNECTION 1
#define EVENT_TYPE_HANDLE_REQUEST 2

typedef void (*RequestHandler)(WormtongueServerContext *server_ctx,
                               Request *request,
                               Response *response);

WormtongueServerContext init_server(int port);

void start_server(WormtongueServerContext *ctx, RequestHandler handle_request);

void shutdown_server(WormtongueServerContext *ctx);

void send_response(WormtongueServerContext *server_ctx, Request *request,
                   Response *response);

#endif
