#include <wormtongue/wormtongue.h>

#include "http.h"
#include "http/handlers.h"
#include "http/log.h"

#include "../core/blog/author/interface/http/author_routes.h"

#define MATCH_PATH(request, route)                                             \
  strncmp((request)->path, (route).path, strlen((route).path)) == 0

struct route routes[] = {ping_route, sign_up_author_route};

size_t routes_count = sizeof(routes) / sizeof(struct route);

void wormtongue_request_handler(Request *request, Response *response) {
  bool found_path = false, found_method = false;

  log_request(request);

  for (int i = 0; i < routes_count; i++) {
    if (MATCH_PATH(request, routes[i])) {
      found_path = true;

      if (request->method == routes[i].method) {
        found_method = true;

        routes[i].handler(request, response);

        break;
      }
    }
  }

  if (!found_path) {
    not_found_handler(request, response);
  } else if (!found_method) {
    unsupported_method_handler(request, response);
  }

  log_response(request, response);
}
