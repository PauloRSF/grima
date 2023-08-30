#include <errno.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <log.h>

#include "app/app.c"

#define DEFAULT_PORT 3000

AppContext ctx;

void shutdown_handler(int signal) {
  if (signal == SIGSEGV) {
    log_fatal("Shutting down due to segfault...");
  } else {
    log_info("Shutting down...");
  }

  shutdown_app(ctx);

  exit(signal);
}

int main(int argc, char **argv) {
  setvbuf(stdout, NULL, _IONBF, 0);

  signal(SIGINT, shutdown_handler);
  signal(SIGSEGV, shutdown_handler);

  int port_argument = argc > 1 ? atoi(argv[1]) : 0;

  int port = port_argument != 0 ? port_argument : DEFAULT_PORT;

  start_app(port);

  return 0;
}