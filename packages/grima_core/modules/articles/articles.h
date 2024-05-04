#ifndef GRIMA_ARTICLES_H
#define GRIMA_ARTICLES_H

#include "../../../shared/dates/date.h"

typedef struct article {
  char* slug;
  char *title;
  char *description;
} Article;

Article *create_article(char* slug, char *title, char *description);

void free_article(Article *article);

Article** find_articles();

#endif