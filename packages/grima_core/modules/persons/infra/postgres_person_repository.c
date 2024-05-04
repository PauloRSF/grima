#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libpq-fe.h>
#include <log.h>
#include <string_list.h>
#include <uuid/uuid.h>

#include "../../../database.h"
#include "../../../shared/dates/date.h"
#include "../domain/person.h"

StringList *get_person_stack(PGconn *database_connection, uuid_t id) {
  const char *query = "SELECT * FROM person_techs WHERE person_id = $1";

  const char *paramValues[1];
  int paramLengths[1];
  int paramFormats[1];

  char id_str[UUID_STR_LEN];
  uuid_unparse_lower(id, id_str);
  paramValues[0] = id_str;
  paramLengths[0] = strlen(id_str);
  paramFormats[0] = 0;

  log_debug("%s %s", DATABASE_LOG_TAG, query);

  PGresult *res = PQexecParams(database_connection, query, 1, NULL, paramValues,
                               paramLengths, paramFormats, 0);

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
  const char *query = "SELECT * FROM persons WHERE id = $1";

  const char *paramValues[1];
  int paramLengths[1];
  int paramFormats[1];

  char id_str[UUID_STR_LEN] = {'\0'};
  uuid_unparse_lower(id, id_str);
  paramValues[0] = id_str;
  paramLengths[0] = strlen(id_str);
  paramFormats[0] = 0;

  log_debug("%s %s", DATABASE_LOG_TAG, query);

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

Person *deserialize_person_from_search(PGresult *res, size_t *index,
                                       size_t row_count) {
  char *id_field = PQgetvalue(res, *index, PQfnumber(res, "id"));
  uuid_t person_id = {'\0'};
  uuid_parse(id_field, person_id);

  char *name_field = PQgetvalue(res, *index, PQfnumber(res, "name"));
  char *nickname_field = PQgetvalue(res, *index, PQfnumber(res, "nickname"));

  char *date_of_birth_field =
      PQgetvalue(res, *index, PQfnumber(res, "date_of_birth"));
  Date date_of_birth = date_from_string(date_of_birth_field);

  StringList *stack = StringList_new();
  char *current_id_field = id_field;

  while (strcmp(current_id_field, id_field) == 0) {
    char *tech_name_field =
        PQgetvalue(res, *index, PQfnumber(res, "tech_name"));

    StringList_add(stack, tech_name_field);

    if (++(*index) < row_count) {
      current_id_field = PQgetvalue(res, *index, PQfnumber(res, "id"));
    } else {
      break;
    }
  }

  Person *person = create_person(person_id, name_field, nickname_field,
                                 date_of_birth, stack);

  StringList_free(stack);

  return person;
}

PersonSearchResult person_repository_search(PGconn *database_connection,
                                            char *search_term) {
  const char *query =
      "SELECT p1.*, pt1.name as \"tech_name\" FROM persons AS p1 JOIN "
      "person_techs AS pt1 on "
      "pt1.person_id=p1.id WHERE id IN (SELECT DISTINCT id FROM persons AS p "
      "JOIN person_techs AS pt ON pt.person_id=p.id WHERE p.name LIKE $1 OR "
      "p.nickname LIKE $1 OR pt.name LIKE $1)";

  char *paramValues[1];
  int paramLengths[1];
  int paramFormats[1];

  paramValues[0] = malloc(strlen(search_term) + 3);
  sprintf((char *)paramValues[0], "%%%s%%", search_term);
  paramLengths[0] = strlen(search_term) + 2;
  paramFormats[0] = 0;

  log_debug("%s %s", DATABASE_LOG_TAG, query);

  PGresult *res =
      PQexecParams(database_connection, query, 1, NULL,
                   (const char **)paramValues, paramLengths, paramFormats, 0);

  free(paramValues[0]);

  PersonSearchResult result = {.persons = NULL, .persons_count = 0};

  if (PQresultStatus(res) != PGRES_TUPLES_OK) {
    fprintf(stderr, "SELECT failed: %s", PQerrorMessage(database_connection));
    PQclear(res);

    return result;
  }

  size_t row_count = PQntuples(res);

  if (row_count < 1)
    return result;

  result.persons = NULL;

  size_t current_row_index = 0;

  while (current_row_index < row_count) {
    Person *person =
        deserialize_person_from_search(res, &current_row_index, row_count);

    result.persons =
        realloc(result.persons, sizeof(Person *) * (result.persons_count + 1));
    result.persons[result.persons_count] = person;
    result.persons_count++;
  }

  PQclear(res);

  return result;
}

bool person_repository_is_nickname_taken(PGconn *database_connection,
                                         char *nickname) {
  const char *query = "SELECT * FROM persons WHERE nickname = $1";

  const char *paramValues[1];
  int paramLengths[1];
  int paramFormats[1];

  paramValues[0] = nickname;
  paramLengths[0] = strlen(nickname);
  paramFormats[0] = 0;

  log_debug("%s %s", DATABASE_LOG_TAG, query);

  PGresult *res = PQexecParams(database_connection, query, 1, NULL, paramValues,
                               paramLengths, paramFormats, 0);

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
  const char *query = "SELECT COUNT(id) AS persons_count FROM persons";

  log_debug("%s %s", DATABASE_LOG_TAG, query);

  PGresult *res = PQexec(database_connection, query);

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

  size_t count = atoi(count_field);

  PQclear(res);

  return count;
}

StringList *insert_person_stack(PGconn *database_connection, uuid_t person_id,
                                StringList *stack) {
  char query[512] = "INSERT INTO person_techs (person_id, name) VALUES ";

  size_t param_index = 1;
  StringListNode *item = NULL;

  size_t items_count = StringList_count(stack);
  char term[16] = {'\0'};

  StringList_ForEach(item, stack) {
    if (param_index == (items_count * 2) - 1)
      break;

    sprintf(term, "($%lu,$%lu),", param_index, param_index + 1);
    strcat(query, term);
    param_index += 2;
  }

  sprintf(term, "($%lu,$%lu)", param_index, param_index + 1);
  strcat(query, term);

  strcat(query, " RETURNING *");

  StringList *saved_stack = StringList_new();

  const char *paramValues[items_count * 2];
  int paramLengths[items_count * 2];
  int paramFormats[items_count * 2];

  char id_str[UUID_STR_LEN];
  uuid_unparse_lower(person_id, id_str);

  size_t index = 0;

  StringList_ForEach(item, stack) {
    paramValues[index] = id_str;
    paramLengths[index] = strlen(id_str);
    paramFormats[index] = 0;

    paramValues[index + 1] = item->data;
    paramLengths[index + 1] = strlen(item->data);
    paramFormats[index + 1] = 0;

    index += 2;
  }

  log_debug("%s %s", DATABASE_LOG_TAG, query);

  PGresult *res =
      PQexecParams(database_connection, query, (items_count * 2), NULL,
                   paramValues, paramLengths, paramFormats, 0);

  if (PQresultStatus(res) != PGRES_TUPLES_OK) {
    fprintf(stderr, "INSERT failed: %s", PQerrorMessage(database_connection));
    PQclear(res);
    StringList_free(saved_stack);

    return NULL;
  }

  for (int i = 0; i < PQntuples(res); ++i) {
    char *name_field = PQgetvalue(res, i, PQfnumber(res, "name"));

    char *name = malloc(strlen(name_field) + 1);
    strcpy(name, name_field);

    StringList_add(saved_stack, name);
  }

  PQclear(res);

  return saved_stack;
}

Person *person_repository_store(PGconn *database_connection, Person *person) {
  const char *query = "INSERT INTO persons (id, name, nickname, date_of_birth) "
                      "VALUES ($1, $2, $3, $4::date) RETURNING *";

  const char *paramValues[4];
  int paramLengths[4];
  int paramFormats[4];

  char id_str[UUID_STR_LEN];
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

  log_debug("%s %s", DATABASE_LOG_TAG, query);

  PGresult *res = PQexecParams(database_connection, query, 4, NULL, paramValues,
                               paramLengths, paramFormats, 0);

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
      person->stack != NULL
          ? insert_person_stack(database_connection, person_id, person->stack)
          : NULL;

  char *name_field = PQgetvalue(res, 0, PQfnumber(res, "name"));
  char *nickname_field = PQgetvalue(res, 0, PQfnumber(res, "nickname"));
  char *date_of_birth_field =
      PQgetvalue(res, 0, PQfnumber(res, "date_of_birth"));

  Date date_of_birth = date_from_string(date_of_birth_field);

  Person *stored_person = create_person(person_id, name_field, nickname_field,
                                        date_of_birth, stack);

  StringList_free(stack);
  PQclear(res);

  return stored_person;
}