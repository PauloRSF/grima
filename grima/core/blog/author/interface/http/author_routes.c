#include <wormtongue/wormtongue.h>

#include <grima/app.h>

void sign_up_handler(Request *request, Response *response) {
  response->status_code = 201;
  response->body = "Author signed up";

  send_response(request, response);
};
