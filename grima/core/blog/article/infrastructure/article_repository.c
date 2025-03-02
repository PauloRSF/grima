#include <stdlib.h>
#include <string.h>

#include <cpino.h>
#include <libpq-fe.h>
#include <uuid/uuid.h>

#include <lib/database.h>
#include <shared/date.h>

#include "../../author/author.h"
#include "../article.h"

#define ARTICLE_PARAMS_COUNT 8

char *ArticleRepository_get_next_id() {
  uuid_t article_id;

  uuid_generate(article_id);

  char *article_id_string = malloc(37 * sizeof(char));

  uuid_unparse(article_id, article_id_string);

  return article_id_string;
};

struct article_params {
  const char *values[ARTICLE_PARAMS_COUNT];
  int lengths[ARTICLE_PARAMS_COUNT];
  int formats[ARTICLE_PARAMS_COUNT];
};

struct article_params build_article_params(struct article *article) {
  struct article_params params = {
      .values = {0},
      .lengths = {0},
      .formats = {0},
  };

  params.values[0] = article->id;
  params.lengths[0] = strlen(article->id);
  params.formats[0] = 0;

  params.values[1] = article->author_id;
  params.lengths[1] = strlen(article->author_id);
  params.formats[1] = 0;

  params.values[2] = article->slug;
  params.lengths[2] = strlen(article->slug);
  params.formats[2] = 0;

  params.values[3] = article->title;
  params.lengths[3] = strlen(article->title);
  params.formats[3] = 0;

  params.values[4] = article->description;
  params.lengths[4] = strlen(article->description);
  params.formats[4] = 0;

  params.values[5] = article->body;
  params.lengths[5] = strlen(article->body);
  params.formats[5] = 0;

  params.values[6] = unix_timestamp_to_iso8601(article->updated_at);
  params.lengths[6] = ISO_TIME_LENGTH;
  params.formats[6] = 0;

  params.values[7] = unix_timestamp_to_iso8601(article->updated_at);
  params.lengths[7] = ISO_TIME_LENGTH;
  params.formats[7] = 0;

  return params;
}

enum article_repository_save_error map_article_save_postgres_error_to_domain_error(PGresult *result) {
  char *error_type = PQresultErrorField(result, PG_DIAG_SQLSTATE);

  bool is_unique_violation = strcmp(error_type, POSTGRES_UNIQUE_VIOLATION_ERROR_CODE) == 0;

  if (!is_unique_violation)
    return ARTICLE_REPOSITORY_SAVE_APPLICATION_ERROR;

  char *constraint_name = PQresultErrorField(result, PG_DIAG_CONSTRAINT_NAME);

  if (strcmp(constraint_name, "article_unique_slug") == 0)
    return ARTICLE_REPOSITORY_SAVE_SLUG_ALREADY_TAKEN;

  return ARTICLE_REPOSITORY_SAVE_APPLICATION_ERROR;
}

struct article_repository_save_result ArticleRepository_save(struct article *article) {
  PGconn *connection = get_database_connection();
  struct article_repository_save_result save_result;

  struct author_repository_exists_by_id_result author_exists_result = AuthorRepository_exists_by_id(article->author_id);

  if (!author_exists_result.success) {
    save_result.success = false;
    save_result.value.error = ARTICLE_REPOSITORY_SAVE_APPLICATION_ERROR;

    return save_result;
  } else if (author_exists_result.value.success == false) {
    save_result.success = false;
    save_result.value.error = ARTICLE_REPOSITORY_SAVE_AUTHOR_NOT_FOUND;

    return save_result;
  }

  const char *query = "INSERT INTO articles (id, author_id, slug, title, description, body, "
                      "created_at, updated_at) "
                      "VALUES ($1, $2, $3, $4, $5, $6, $7, $8)";

  struct article_params params = build_article_params(article);

  cpino_log_debug("[DATABASE] %s", query);

  PGresult *result =
      PQexecParams(connection, query, ARTICLE_PARAMS_COUNT, NULL, params.values, params.lengths, params.formats, 0);

  if (PQresultStatus(result) == PGRES_COMMAND_OK) {
    PQclear(result);

    save_result.success = true;

    return save_result;
  }

  char *message = PQresultVerboseErrorMessage(result, PQERRORS_VERBOSE, PQSHOW_CONTEXT_ALWAYS);

  cpino_log_error("[DATABASE] Failed to insert Article: %s", message);

  PQfreemem(message);

  save_result.success = false;
  save_result.value.error = map_article_save_postgres_error_to_domain_error(result);

  PQclear(result);

  return save_result;
}

struct article_repository_exists_by_id_result ArticleRepository_exists_by_id(char *article_id) {
  PGconn *connection = get_database_connection();

  const char *query = "SELECT COUNT(*) FROM articles WHERE id = $1";

  const char *values[1] = {article_id};
  int lengths[1] = {strlen(article_id)};
  int formats[1] = {0};

  cpino_log_debug("[DATABASE] %s", query);

  PGresult *result = PQexecParams(connection, query, 1, NULL, values, lengths, formats, 0);

  if (PQresultStatus(result) != PGRES_TUPLES_OK) {
    char *message = PQresultVerboseErrorMessage(result, PQERRORS_VERBOSE, PQSHOW_CONTEXT_ALWAYS);

    cpino_log_error("[DATABASE] Failed to check if Article exists: %s", message);

    PQfreemem(message);

    PQclear(result);

    return (struct article_repository_exists_by_id_result){
        .success = false,
    };
  }

  char *count_string = PQgetvalue(result, 0, 0);

  bool exists = atoi(count_string) > 0;

  PQclear(result);

  return (struct article_repository_exists_by_id_result){
      .success = true,
      .value =
          {
              .success = exists,
          },
  };
}
