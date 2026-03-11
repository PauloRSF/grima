#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <cpino.h>
#include <libpq-fe.h>

#include "database.h"

PGconn *database_connection = NULL;

PGconn *get_database_connection() {
  if (!database_connection) {
    cpino_log_error("[DATABASE] Database is not connected");
    exit(9);
  } else if (PQstatus(database_connection) != CONNECTION_OK) {
    cpino_log_error("[DATABASE] Database connection is not healthy");
    exit(9);
  }

  return database_connection;
}

struct pg_query_params new_pg_query_params() {
  return (struct pg_query_params){
      .count = 0,
      .values = NULL,
      .lengths = NULL,
      .formats = NULL,
  };
}

void add_pg_query_param(struct pg_query_params *params, char *value) {
  ++params->count;

  size_t new_param_index = params->count - 1;

  params->values = realloc(params->values, params->count * sizeof(char *));
  params->lengths = realloc(params->lengths, params->count * sizeof(int));
  params->formats = realloc(params->formats, params->count * sizeof(int));

  size_t value_length = value ? strlen(value) : 0;

  if (value) {
    params->values[new_param_index] = calloc(sizeof(char), value_length + 1);
    strcpy(params->values[new_param_index], value);
  } else {
    params->values[new_param_index] = NULL;
  }

  params->lengths[new_param_index] = value_length;
  params->formats[new_param_index] = 0;
}

void free_pg_query_params(struct pg_query_params params) {
  for (size_t i = 0; i < params.count; i++)
    free(params.values[i]);

  free(params.values);
  free(params.lengths);
  free(params.formats);
}

PGresult *pg_query(char *query, struct pg_query_params params) {
  cpino_log_debug("[DATABASE] %s", query);

  PGresult *result = PQexecParams(database_connection, query, params.count, NULL, (const char *const *)params.values,
                                  params.lengths, params.formats, 0);

  ExecStatusType status = PQresultStatus(result);
  bool has_query_failed = status != PGRES_TUPLES_OK && status != PGRES_COMMAND_OK;

  if (has_query_failed) {
    char *message = PQresultVerboseErrorMessage(result, PQERRORS_VERBOSE, PQSHOW_CONTEXT_ALWAYS);

    cpino_log_error("[DATABASE] Failed to run query: %s", message);

    PQfreemem(message);
  }

  return result;
}

bool has_constraint_violation(PGresult *result, char *constraint_name) {
  if (PQresultStatus(result) != PGRES_FATAL_ERROR)
    return false;

  char *error_type = PQresultErrorField(result, PG_DIAG_SQLSTATE);

  bool is_constraint_violation = strcmp(error_type, POSTGRES_INTEGRITY_CONSTRAINT_VIOLATION_ERROR_CODE) == 0 ||
                                 strcmp(error_type, POSTGRES_RESTRICT_VIOLATION_ERROR_CODE) == 0 ||
                                 strcmp(error_type, POSTGRES_NOT_NULL_VIOLATION_ERROR_CODE) == 0 ||
                                 strcmp(error_type, POSTGRES_FOREIGN_KEY_VIOLATION_ERROR_CODE) == 0 ||
                                 strcmp(error_type, POSTGRES_UNIQUE_VIOLATION_ERROR_CODE) == 0 ||
                                 strcmp(error_type, POSTGRES_CHECK_VIOLATION_ERROR_CODE) == 0 ||
                                 strcmp(error_type, POSTGRES_EXCLUSION_VIOLATION_ERROR_CODE) == 0;

  if (!is_constraint_violation)
    return false;

  char *violated_constraint_name = PQresultErrorField(result, PG_DIAG_CONSTRAINT_NAME);

  return strcmp(violated_constraint_name, constraint_name) == 0;
}

bool matches_foreign_key_violation(PGresult *result, char *constraint_name) {
  char *error_type = PQresultErrorField(result, PG_DIAG_SQLSTATE);

  bool is_foreign_key_violation = strcmp(error_type, POSTGRES_FOREIGN_KEY_VIOLATION_ERROR_CODE) == 0;

  if (!is_foreign_key_violation)
    return false;

  char *violated_constraint_name = PQresultErrorField(result, PG_DIAG_CONSTRAINT_NAME);

  return strcmp(violated_constraint_name, constraint_name) == 0;
}

bool connect_database(char *connection_uri) {
  cpino_log_info("[DATABASE] Connecting to database");

  PGconn *connection = PQconnectdb(connection_uri);

  if (PQstatus(connection) != CONNECTION_OK) {
    cpino_log_error("Could not connect to database: %s", PQerrorMessage(connection));

    PQfinish(connection);

    return false;
  }

  cpino_log_info("[DATABASE] Database connected");

  struct pg_query_params params = new_pg_query_params();

  database_connection = connection;

  PGresult *result = pg_query("SELECT 1+1", params);

  bool can_run_queries = PQresultStatus(result) == PGRES_TUPLES_OK;

  PQclear(result);

  if (!can_run_queries) {
    cpino_log_error("[DATABASE] Database can not run queries");

    PQfinish(connection);

    return false;
  }

  return true;
}

void shutdown_database() { PQfinish(database_connection); }
