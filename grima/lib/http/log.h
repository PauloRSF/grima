#ifndef GRIMA_LIB_HTTP_LOG_H
#define GRIMA_LIB_HTTP_LOG_H

#include <wormtongue/wormtongue.h>

void log_request(Request *request);

void log_response(Request *request, Response *response);

#endif
