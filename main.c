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

struct server_context ctx;

int client_socket_descriptor;

void shutdown_handler() {
  printf("\nShutting down...\n");

  close(client_socket_descriptor);

  shutdown_server(ctx);

  exit(130);
}

void handle_request(Request request) {
  Headers headers = create_headers();

  add_header(headers, "Connection", "close");

  if (strcmp(request.path, "/healthz") == 0) {
    add_header(headers, "Content-Type", "application/json");

    send_response(ctx, 200, headers, "{\"msg\": \"healthy\"}");
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

    handle_request(request);

    free_request(&request);
  }

  return 0;
}