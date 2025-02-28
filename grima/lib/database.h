#ifndef GRIMA_LIB_DATABASE_H
#define GRIMA_LIB_DATABASE_H

#include <libpq-fe.h>

#define POSTGRES_CONSTRAINT_VIOLATION_ERROR_CODE "23505"

PGconn *get_database_connection();

void set_database_connection(PGconn *connection);

#endif
