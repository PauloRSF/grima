#ifndef GRIMA_LIB_DATABASE_H
#define GRIMA_LIB_DATABASE_H

#include <libpq-fe.h>

#define POSTGRES_UNIQUE_VIOLATION_ERROR_CODE "23505"
#define POSTGRES_FOREIGN_KEY_VIOLATION_ERROR_CODE "23503"

PGconn *get_database_connection();

void set_database_connection(PGconn *connection);

#endif
