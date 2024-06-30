#ifndef WORMTONGUE_SERVER_H
#define WORMTONGUE_SERVER_H

#include <netinet/in.h>

#include "headers.h"
#include "request.h"
#include "response.h"

#define CLIENT_RECEIVE_BUFFER_SIZE 2048

typedef void (*Logger)(const char *format, ...);

typedef struct server_context {
  int server_descriptor;
  Logger logger;
} WormtongueServerContext;

typedef void (*RequestHandler)(WormtongueServerContext *ctx, Request *request,
                               Response *response);

WormtongueServerContext setup_server(int port, Logger logger);

void start_server(WormtongueServerContext *ctx, RequestHandler handle_request,
                  bool blocking);

void shutdown_server(WormtongueServerContext *ctx);

void send_response(WormtongueServerContext *ctx, Request *request,
                   Response *response);

#endif
