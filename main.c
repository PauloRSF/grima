#include <errno.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "app/app.c"

AppContext ctx;

void shutdown_handler() {
  printf("\nShutting down...\n");

  shutdown_app(ctx);

  exit(SIGINT);
}

int main(int argc, char **argv) {
  setvbuf(stdout, NULL, _IONBF, 0);

  signal(SIGINT, shutdown_handler);
  signal(SIGSEGV, shutdown_handler);

  start_app(atoi(argv[1]));

  return 0;
}