// #include <bits/types/sigset_t.h>
// #include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libpq-fe.h>
#include <string_list.h>
#include <uuid/uuid.h>

#include "../../../database.h"
#include "../../../shared_kernel/dates/date.h"
#include "../domain/person.h"

StringList *get_person_stack(PGconn *database_connection, uuid_t id) {

  const char *paramValues[1];
  int paramLengths[1];
  int paramFormats[1];

  char id_str[37];
  uuid_unparse_lower(id, id_str);
  paramValues[0] = id_str;
  paramLengths[0] = strlen(id_str);
  paramFormats[0] = 0;

  PGresult *res = PQexecParams(
      database_connection, "SELECT * FROM person_techs WHERE person_id = $1", 1,
      NULL, paramValues, paramLengths, paramFormats, 0);

  if (PQresultStatus(res) != PGRES_TUPLES_OK) {
    fprintf(stderr, "SELECT failed: %s", PQerrorMessage(database_connection));
    PQclear(res);

    return NULL;
  }

  if (PQntuples(res) < 1) {
    PQclear(res);

    return NULL;
  }

  StringList *techs = StringList_new();

  for (int i = 0; i < PQntuples(res); ++i) {
    char *name_field = PQgetvalue(res, i, PQfnumber(res, "name"));

    char *name = malloc(strlen(name_field) + 1);
    strcpy(name, name_field);

    StringList_add(techs, name);
  }

  PQclear(res);

  return techs;
}

Person *person_repository_get_by_id(PGconn *database_connection, uuid_t id) {
  const char *paramValues[1];
  int paramLengths[1];
  int paramFormats[1];

  char id_str[37] = {'\0'};
  uuid_unparse_lower(id, id_str);
  paramValues[0] = id_str;
  paramLengths[0] = strlen(id_str);
  paramFormats[0] = 0;

  PGresult *res =
      PQexecParams(database_connection, "SELECT * FROM persons WHERE id = $1",
                   1, NULL, paramValues, paramLengths, paramFormats, 0);

  if (PQresultStatus(res) != PGRES_TUPLES_OK) {
    fprintf(stderr, "SELECT failed: %s", PQerrorMessage(database_connection));
    PQclear(res);

    return NULL;
  }

  if (PQntuples(res) < 1) {
    PQclear(res);

    return NULL;
  }

  char *id_field = PQgetvalue(res, 0, PQfnumber(res, "id"));
  char *name_field = PQgetvalue(res, 0, PQfnumber(res, "name"));
  char *nickname_field = PQgetvalue(res, 0, PQfnumber(res, "nickname"));
  char *date_of_birth_field =
      PQgetvalue(res, 0, PQfnumber(res, "date_of_birth"));

  Date date_of_birth = date_from_string(date_of_birth_field);

  StringList *stack = get_person_stack(database_connection, id);

  uuid_t person_id = {'\0'};
  uuid_parse(id_field, person_id);

  Person *person = create_person(person_id, name_field, nickname_field,
                                 date_of_birth, stack);

  StringList_free(stack);

  PQclear(res);

  return person;
}

bool person_repository_is_nickname_taken(PGconn *database_connection,
                                         char *nickname) {
  const char *paramValues[1];
  int paramLengths[1];
  int paramFormats[1];

  paramValues[0] = nickname;
  paramLengths[0] = strlen(nickname);
  paramFormats[0] = 0;

  PGresult *res = PQexecParams(
      database_connection, "SELECT * FROM persons WHERE nickname = $1", 1, NULL,
      paramValues, paramLengths, paramFormats, 0);

  if (PQresultStatus(res) != PGRES_TUPLES_OK) {
    fprintf(stderr, "SELECT failed: %s", PQerrorMessage(database_connection));
    PQclear(res);

    return false;
  }

  if (PQntuples(res) < 1) {
    PQclear(res);

    return false;
  }

  PQclear(res);

  return true;
}

size_t person_repository_count(PGconn *database_connection) {
  PGresult *res = PQexec(database_connection,
                         "SELECT COUNT(id) as persons_count FROM persons");

  if (PQresultStatus(res) != PGRES_TUPLES_OK) {
    fprintf(stderr, "SELECT failed: %s", PQerrorMessage(database_connection));
    PQclear(res);

    return 0;
  }

  if (PQntuples(res) < 1) {
    PQclear(res);

    return 0;
  }

  char *count_field = PQgetvalue(res, 0, PQfnumber(res, "persons_count"));

  PQclear(res);

  return atoi(count_field);
}

StringList *insert_person_stack(PGconn *database_connection, uuid_t person_id,
                                StringList *stack) {
  if (stack == NULL)
    return NULL;

  StringList *saved_stack = StringList_new();
  StringListNode *item = NULL;

  StringList_ForEach(item, stack) {
    const char *paramValues[2];
    int paramLengths[2];
    int paramFormats[2];

    char id_str[37];
    uuid_unparse_lower(person_id, id_str);
    paramValues[0] = id_str;
    paramLengths[0] = strlen(id_str);
    paramFormats[0] = 0;

    paramValues[1] = item->data;
    paramLengths[1] = strlen(item->data);
    paramFormats[1] = 0;

    PGresult *res =
        PQexecParams(database_connection,
                     "INSERT INTO person_techs (person_id, name) "
                     "VALUES ($1, $2) RETURNING *",
                     2, NULL, paramValues, paramLengths, paramFormats, 0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
      fprintf(stderr, "INSERT failed: %s", PQerrorMessage(database_connection));
      PQclear(res);
      free(saved_stack);

      return NULL;
    }

    PQclear(res);
  }

  return saved_stack;
}

Person *person_repository_store(PGconn *database_connection, Person *person) {
  const char *paramValues[4];
  int paramLengths[4];
  int paramFormats[4];

  char id_str[37];
  uuid_unparse_lower(person->id, id_str);
  paramValues[0] = id_str;
  paramLengths[0] = strlen(id_str);
  paramFormats[0] = 0;

  paramValues[1] = person->name;
  paramLengths[1] = strlen(person->name);
  paramFormats[1] = 0;

  paramValues[2] = person->nickname;
  paramLengths[2] = strlen(person->name);
  paramFormats[2] = 0;

  char *date_of_birth_string = date_to_string(person->date_of_birth);

  paramValues[3] = date_of_birth_string;
  paramLengths[3] = strlen(date_of_birth_string);
  paramFormats[3] = 0;

  PGresult *res =
      PQexecParams(database_connection,
                   "INSERT INTO persons (id, name, nickname, date_of_birth) "
                   "VALUES ($1, $2, $3, $4::date) RETURNING *",
                   4, NULL, paramValues, paramLengths, paramFormats, 0);

  free(date_of_birth_string);

  if (PQresultStatus(res) != PGRES_TUPLES_OK) {
    fprintf(stderr, "INSERT failed: %s", PQerrorMessage(database_connection));
    PQclear(res);

    return NULL;
  }

  char *id_field = PQgetvalue(res, 0, PQfnumber(res, "id"));

  uuid_t person_id = {'\0'};
  uuid_parse(id_field, person_id);

  StringList *stack =
      insert_person_stack(database_connection, person_id, person->stack);

  char *name_field = PQgetvalue(res, 0, PQfnumber(res, "name"));
  char *nickname_field = PQgetvalue(res, 0, PQfnumber(res, "nickname"));
  char *date_of_birth_field =
      PQgetvalue(res, 0, PQfnumber(res, "date_of_birth"));

  Date date_of_birth = date_from_string(date_of_birth_field);

  Person *stored_person = create_person(person_id, name_field, nickname_field,
                                        date_of_birth, stack);

  free(stack);
  PQclear(res);

  return stored_person;
}