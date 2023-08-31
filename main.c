#include <errno.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <log.h>

#include "app/app.h"

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

  char *port_env_var = getenv("PORT");

  int port = port_env_var != NULL ? atoi(port_env_var) : DEFAULT_PORT;

  start_app(port, &ctx);

  return 0;
}