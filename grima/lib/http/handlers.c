#include <wormtongue/wormtongue.h>

void unsupported_method_handler(Request *request, Response *response) {
  response->status_code = 405;

  response->body = "Method not allowed";

  add_header(response->headers, "Content-Type", "text/plain");

  send_response(request, response);
}

void not_found_handler(Request *request, Response *response) {
  response->status_code = 404;

  response->body = "Not found";

  add_header(response->headers, "Content-Type", "text/plain");

  send_response(request, response);
}

void ping_handler(Request *request, Response *response) {
  response->status_code = 200;

  response->body = "pong";

  add_header(response->headers, "Content-Type", "text/plain");

  send_response(request, response);
}
