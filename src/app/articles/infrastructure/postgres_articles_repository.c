#include <string.h>

#include <libpq-fe.h>
#include <cpino.h>

#include <grima/articles.h>

#include "../../lib/database/database.h"
#include "article_postgres_mapper.h"

char* build_find_articles_query(FindArticlesOptions options) {
  char *query = malloc(1024);

  strcpy(query, "SELECT * FROM articles");

  if (options.tag || options.author || options.favorited) {
    query = strcat(query, " WHERE");
  }

  if (options.tag) {
    query = strcat(query, " tag = '");
    query = strcat(query, options.tag);
    query = strcat(query, "'");
  }

  if (options.author) {
    query = strcat(query, " author = '");
    query = strcat(query, options.author);
    query = strcat(query, "'");
  }

  if (options.favorited) {
    query = strcat(query, " favorited = '");
    query = strcat(query, options.favorited);
    query = strcat(query, "'");
  }

  char limit_str[20];
  sprintf(limit_str, "%zu", options.limit);
  query = strcat(query, " LIMIT ");
  query = strcat(query, limit_str);

  char offset_str[20];
  sprintf(offset_str, "%zu", options.offset);
  query = strcat(query, " OFFSET ");
  query = strcat(query, offset_str);

  return query;
}

FindArticlesResult ArticleRepository_find_articles(FindArticlesOptions options) {
  PGconn *database_connection = get_database_connection();

  char *query = build_find_articles_query(options);

  cpino_log_debug("DATABASE: %s", query);

  PGresult *query_result = PQexecParams(database_connection, query, 0, NULL, NULL, NULL, NULL, 0);

  free(query);

  FindArticlesResult result = {
    .is_error = false,
    .error_code = 0,
    .count = 0,
    .articles = NULL
  };

  if (PQresultStatus(query_result) != PGRES_TUPLES_OK) {
    result.is_error = true;
    result.error_code = FindArticlesUnknownError;

    cpino_log_error("DATABASE: %s", PQerrorMessage(database_connection));

    PQclear(query_result);

    return result;
  }

  for (int i = 0; i < PQntuples(query_result); i++) {
    Article *article = ArticlePostgresMapper_to_article(query_result, i);

    result.count++;
    result.articles = realloc(result.articles, sizeof(Article) * result.count);

    result.articles[result.count - 1] = article;
  }

  PQclear(query_result);

  return result;
}
