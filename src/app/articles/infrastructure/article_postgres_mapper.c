#include <string.h>

#include <libpq-fe.h>

#include <grima/articles.h>

Article *ArticlePostgresMapper_to_article(PGresult *query_result, size_t row) {
  char *slug = PQgetvalue(query_result, row, PQfnumber(query_result, "slug"));
  char *title = PQgetvalue(query_result, row, PQfnumber(query_result, "title"));
  char *description = PQgetvalue(query_result, row, PQfnumber(query_result, "description"));

  Article *article = (Article *)malloc(sizeof(Article));

  article->slug = malloc(strlen(slug) + 1);
  strcpy(article->slug, slug);

  article->title = malloc(strlen(title) + 1);
  strcpy(article->title, title);

  article->description = malloc(strlen(description) + 1);
  strcpy(article->description, description);

  return article;
}
