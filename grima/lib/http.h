#ifndef GRIMA_LIB_HTTP_H
#define GRIMA_LIB_HTTP_H

#include <string.h>

#include <wormtongue/wormtongue.h>

struct route {
  const char *path;
  Method method;
  void (*handler)(Request *, Response *);
};

void wormtongue_request_handler(Request *, Response *);

#endif
