#ifndef GRIMA_SERVER_H
#define GRIMA_SERVER_H
#include <netinet/in.h>

#include "headers.h"
#include "request.h"
#include "response.h"

#define CLIENT_RECEIVE_BUFFER_SIZE 1024

typedef struct server_context {
  int server_descriptor;
  struct sockaddr_in address;
  int client_socket_descriptor;
} ServerContext;

ServerContext start_server(int port);

void shutdown_server(ServerContext ctx);

Request receive_request(ServerContext *ctx);

void send_response(ServerContext *ctx, Response response);

void accept_connection(ServerContext *ctx,
                       void (*handle_request)(ServerContext *ctx,
                                              Request request,
                                              Response response));
#endif
