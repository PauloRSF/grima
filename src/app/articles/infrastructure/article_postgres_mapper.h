#include <libpq-fe.h>

#include <grima/articles.h>

Article *ArticlePostgresMapper_to_article(PGresult *query_result, size_t row);
