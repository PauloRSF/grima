#ifndef GRIMA_ARTICLES_H
#define GRIMA_ARTICLES_H

#include <stdlib.h>
#include <stdbool.h>

// ----- Article Entity -----

typedef struct article {
  char *slug;
  char *title;
  char *description;
} Article;

Article *create_article(char *title, char *description);

void free_article(Article *article);

// ----- Find Articles Application Service -----

typedef struct find_articles_options {
  char *tag;
  char *author;
  char *favorited;
  size_t limit;
  size_t offset;
} FindArticlesOptions;

typedef enum find_articles_error {
  FindArticlesUnknownError = 0,
} FindArticlesError;

typedef struct find_articles_result {
  bool is_error;
  FindArticlesError error_code;
  Article **articles;
  size_t count;
} FindArticlesResult;

FindArticlesResult find_articles(FindArticlesOptions options);

void free_find_articles_result(FindArticlesResult article);

// ----- Articles Repository -----

FindArticlesResult ArticleRepository_find_articles(FindArticlesOptions options);

#endif
