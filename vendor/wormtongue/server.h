#ifndef WORMTONGUE_SERVER_H
#define WORMTONGUE_SERVER_H

#include <netinet/in.h>

#include "headers.h"
#include "request.h"
#include "response.h"

#define CLIENT_RECEIVE_BUFFER_SIZE 2048

typedef void (*wormtongue_log_fn)(char *format, ...);

typedef struct wormtongue_server_context {
  int server_descriptor;
  wormtongue_log_fn log;
} WormtongueServerContext;

typedef void (*RequestHandler)(Request *request, Response *response);

struct wormtongue_server_context setup_server(int port, wormtongue_log_fn log);

void start_server(struct wormtongue_server_context *ctx,
                  RequestHandler handle_request, bool blocking);

void shutdown_server(struct wormtongue_server_context *ctx);

void send_response(Request *request, Response *response);

#endif
