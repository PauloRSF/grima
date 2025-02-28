#include <errno.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <cpino.h>

#include <grima/app.h>
#include <grima/blog/author.h>

#include "./database/database.h"
#include "./http/http.h"

#define DATABASE_MODULE_INDEX 0
#define HTTP_MODULE_INDEX 1

bool modules_to_shutdown[2] = {false, false};

void mark_module_as_initialized(uint module_index) { modules_to_shutdown[module_index] = true; }

void start_app(AppContext *app_ctx) {
  cpino_log_info("[BOOT] Booting application");

  cpino_log_info("[BOOT] Initializing database");
  bool was_database_setup_successful =
      connect_database("postgres://postgres:postgres@postgres:5432/grima_development");

  if (was_database_setup_successful) {
    cpino_log_info("[BOOT] Database initialized");
  } else {
    cpino_log_fatal("[BOOT] Failed to setup the database. Exiting...");
    return shutdown_app(*app_ctx);
  };

  // cpino_log_info("[BOOT] Initializing HTTP server");
  // bool was_http_server_setup_successful = start_http_server(app_ctx);

  // if (was_http_server_setup_successful) {
  //   cpino_log_info("[BOOT] HTTP server initialized");
  // } else {
  //   cpino_log_fatal("[BOOT] Failed to setup the HTTP server. Exiting...");
  //   return shutdown_app(*app_ctx);
  // };
}

void shutdown_app(AppContext app_ctx) {
  cpino_log_info("[BOOT] Shutting down application");

  // shutdown_server(&app_ctx.server_context);
  // shutdown_database(app_ctx.database_context);
};

AppContext ctx;

bool is_dying = false;

void shutdown_handler(int signal) {
  if (signal == SIGSEGV) {
    cpino_log_fatal("Shutting down due to segfault...");
  } else {
    cpino_log_info("Shutting down...");
  }

  if (is_dying) {
    cpino_log_fatal("Killing application due to previous shutdown signal");
    exit(signal);
  }

  is_dying = true;

  shutdown_app(ctx);

  exit(signal);
}

int main(int argc, char **argv) {
  setvbuf(stdout, NULL, _IONBF, 0);

  signal(SIGINT, shutdown_handler);
  signal(SIGSEGV, shutdown_handler);

  start_app(&ctx);

  struct create_author_result result = Author_create("foobar", "", "");

  if (result.success) {
    struct author *author = result.value.success;

    Author_show(author);

    struct author_repository_save_result res = AuthorRepository_save(author);

    printf("AuthorRepository_save result: %d\n", res);
  } else {
    Author_show_creation_error(result.value.error);
  }

  return 0;
}
