#ifndef GRIMA_SHARED_KERNEL_INTERFACE_HTTP_H
#define GRIMA_SHARED_KERNEL_INTERFACE_HTTP_H

#include "../../error/error.h"
#include <grima_http.h>

void get_response_for_error(Error *error, Response *response);

#endif
