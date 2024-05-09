#include <string.h>

#include <grima_core.h>
#include <wormtongue.h>
#include <grima_app.h>

#define DEFAULT_PORT 3000

#define MATCH_PATH(request_path, path)                                         \
  strncmp((request_path), (path), strlen((path))) == 0

typedef struct route {
  char* path;
  char** methods;
  void (*handler)(AppContext*, Request*, Response*);
} Route;

void start_http_server(AppContext *app_ctx);
