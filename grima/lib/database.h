#ifndef GRIMA_LIB_DATABASE_H
#define GRIMA_LIB_DATABASE_H

#include <stddef.h>
#include <stdbool.h>

#include <libpq-fe.h>

#define POSTGRES_INTEGRITY_CONSTRAINT_VIOLATION_ERROR_CODE "23000"
#define POSTGRES_RESTRICT_VIOLATION_ERROR_CODE "23001"
#define POSTGRES_NOT_NULL_VIOLATION_ERROR_CODE "23502"
#define POSTGRES_FOREIGN_KEY_VIOLATION_ERROR_CODE "23503"
#define POSTGRES_UNIQUE_VIOLATION_ERROR_CODE "23505"
#define POSTGRES_CHECK_VIOLATION_ERROR_CODE "23514"
#define POSTGRES_EXCLUSION_VIOLATION_ERROR_CODE "23P01"

PGconn *get_database_connection();

bool connect_database(char *connection_uri);

void shutdown_database();

struct pg_query_params {
  size_t count;
  char **values;
  int *lengths;
  int *formats;
};

struct pg_query_params new_pg_query_params();
void add_pg_query_param(struct pg_query_params *params, char *value);
void free_pg_query_params(struct pg_query_params params);

PGresult *pg_query(char *query, struct pg_query_params params);

bool has_constraint_violation(PGresult *result, char *constraint_name);

#endif
