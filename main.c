#include <errno.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <cpino.h>

#include <grima_app.h>

AppContext ctx;

void shutdown_handler(int signal) {
  if (signal == SIGSEGV) {
    cpino_log_fatal("Shutting down due to segfault...");
  } else {
    cpino_log_info("Shutting down...");
  }

  shutdown_app(ctx);

  exit(signal);
}

int main(int argc, char **argv) {
  setvbuf(stdout, NULL, _IONBF, 0);

  signal(SIGINT, shutdown_handler);
  signal(SIGSEGV, shutdown_handler);

  start_app(&ctx);

  return 0;
}