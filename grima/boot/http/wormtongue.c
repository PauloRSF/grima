#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <cjson/cJSON.h>

#include <cpino.h>
#include <wormtongue/wormtongue.h>

#include <grima/app.h>
#include <lib/http.h>

#include "http.h"

bool start_http_server(AppContext *app_ctx) {
  char *port_env_var = getenv("PORT");

  int port = port_env_var != NULL ? atoi(port_env_var) : DEFAULT_PORT;

  cpino_log_info("[HTTP] Starting server");

  struct wormtongue_server_context server_context = setup_server(port, &printf);

  start_server(&server_context, wormtongue_request_handler, false);

  cpino_log_info("[HTTP] Listening in port %d", port);

  return true;
}
