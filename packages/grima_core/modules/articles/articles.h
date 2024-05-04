#ifndef GRIMA_ARTICLES_H
#define GRIMA_ARTICLES_H

#include "../../shared/dates/date.h"

typedef struct article {
  char *slug;
  char *title;
  char *description;
} Article;

Article *create_article(char* slug, char *title, char *description);

void free_article(Article *article);

typedef struct find_articles_result {
  Article **articles;
  size_t count;
} FindArticlesResult;

FindArticlesResult find_articles();

void free_find_articles_result(FindArticlesResult article);

#endif
