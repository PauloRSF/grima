#include <stdbool.h>
#include <stdlib.h>

#include <cpino.h>

#include <grima/app.h>
#include <grima/auth.h>

#include "./crypto/crypto.h"
#include "./database/database.h"
#include "./http/http.h"

#define DATABASE_MODULE_INDEX 0
#define HTTP_MODULE_INDEX 1

bool modules_to_shutdown[2] = {false, false};

void mark_module_as_initialized(uint module_index) { modules_to_shutdown[module_index] = true; }

void start_app(AppContext *app_ctx) {
  cpino_log_info("[BOOT] Booting application");

  cpino_log_info("[BOOT] Initializing crypto");
  bool was_crypto_setup_successful = setup_crypto();

  if (was_crypto_setup_successful) {
    cpino_log_info("[BOOT] Crypto initialized");
  } else {
    cpino_log_fatal("[BOOT] Failed to setup crypto functionality. Exiting...");
    return shutdown_app(*app_ctx);
  };

  cpino_log_info("[BOOT] Initializing database");
  bool was_database_setup_successful =
      connect_database("postgres://postgres:postgres@postgres:5432/grima_development");

  if (was_database_setup_successful) {
    cpino_log_info("[BOOT] Database initialized");
  } else {
    cpino_log_fatal("[BOOT] Failed to setup the database. Exiting...");
    return shutdown_app(*app_ctx);
  };

  cpino_log_info("[BOOT] Initializing HTTP server");
  bool was_http_server_setup_successful = start_http_server(app_ctx);

  if (was_http_server_setup_successful) {
    cpino_log_info("[BOOT] HTTP server initialized");
  } else {
    cpino_log_fatal("[BOOT] Failed to setup the HTTP server. Exiting...");
    return shutdown_app(*app_ctx);
  };

  struct create_account_result result = Account_create("email@foo.bar", "foobar", "Foo123foo!");

  if (result.success) {
    struct account *account = result.success_value;

    Account_pretty_print(account);

    enum account_repository_save_result res = AccountRepository_save(result.success_value);

    printf("AccountRepository_save result: %d\n", res);
  } else {
    struct account_validation_error *errors = result.error_value;

    for (size_t i = 0; i < 2; i++) {
      struct account_validation_error error = errors[i];

      printf("Error: [%s] %s\n", error.key, error.message);
    }
  }

  // getchar();
}

void shutdown_app(AppContext app_ctx) {
  cpino_log_info("[BOOT] Shutting down application");

  // shutdown_server(&app_ctx.server_context);
  // shutdown_database(app_ctx.database_context);
};
