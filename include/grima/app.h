#ifndef GRIMA_APP_H
#define GRIMA_APP_H

typedef struct app_context {} AppContext;

void start_app(AppContext *ctx);

void shutdown_app(AppContext ctx);

#endif
