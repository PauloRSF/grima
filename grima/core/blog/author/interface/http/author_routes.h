#include <lib/http.h>

#include <wormtongue/wormtongue.h>

void sign_up_handler(Request *request, Response *response);

const struct route sign_up_author_route = {
    .path = "/api/users",
    .method = POST,
    .handler = sign_up_handler,
};
