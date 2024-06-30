#ifndef GRIMA_LIB_DATABASE_H
#define GRIMA_LIB_DATABASE_H

#include <libpq-fe.h>

PGconn *get_database_connection();

void set_database_connection(PGconn *connection);

#endif
