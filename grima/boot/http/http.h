#include <stdbool.h>
#include <string.h>

#include <wormtongue/request.h>

#include <grima/app.h>

#define DEFAULT_PORT 3000

#define MATCH_PATH(request_path, path)                                         \
  strncmp((request_path), (path), strlen((path))) == 0

typedef struct route {
  char *path;
  Method *methods;
  void (*handler)(AppContext *, Request *, Response *);
} Route;

bool start_http_server(AppContext *app_ctx);
