#ifndef GRIMA_LIB_HTTP_HANDLERS_H
#define GRIMA_LIB_HTTP_HANDLERS_H

#include <wormtongue/wormtongue.h>

void unsupported_method_handler(Request *request, Response *response);

void not_found_handler(Request *request, Response *response);

void ping_handler(Request *request, Response *response);

const struct route ping_route = {
    .path = "/ping", .method = GET, .handler = ping_handler};

#endif
