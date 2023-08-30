#ifndef GRIMA_SHARED_KERNEL_INTERFACE_HTTP_H
#define GRIMA_SHARED_KERNEL_INTERFACE_HTTP_H

#include "../../../../http/response.h"
#include "../../error/error.h"

void get_response_for_error(Error *error, Response *response);

#endif
