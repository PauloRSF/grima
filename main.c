#include <errno.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "headers.c"
#include "request.c"
#include "response.c"
#include "server.c"

ServerContext ctx;

void shutdown_handler() {
  printf("\nShutting down...\n");

  shutdown_server(ctx);

  exit(SIGINT);
}

void handle_healthz_endpoint(ServerContext ctx, Request request,
                             Headers headers) {
  if (request.method != GET)
    return send_response(ctx, 405, headers, NULL);

  add_header(headers, "Content-Type", "application/json");

  return send_response(ctx, 200, headers, "{\"msg\": \"healthy\"}");
}

void handle_request(ServerContext ctx, Request request) {
  Headers headers = create_headers();

  add_header(headers, "Connection", "close");

  if (strcmp(request.path, "/healthz") == 0) {
    handle_healthz_endpoint(ctx, request, headers);
  } else {
    add_header(headers, "Content-Length", "0");

    send_response(ctx, 404, headers, NULL);
  }

  free_headers(headers);
}

int main(int argc, char **argv) {
  signal(SIGINT, shutdown_handler);
  signal(SIGSEGV, shutdown_handler);

  printf("Starting server...\n");

  ctx = start_server(PORT);

  printf("Listening in port %d\n\n", PORT);

  while (true) {
    Request request = receive_request(&ctx);

    printf("%s %s\n", get_method_name(request.method), request.path);

    handle_request(ctx, request);

    free_request(&request);
  }

  return 0;
}