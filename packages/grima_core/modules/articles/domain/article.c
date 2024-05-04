#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../../shared/dates/date.h"
#include "article.h"

Person *create_article(char* slug, char *title, char *description) {
  Article *article = (Article *)calloc(1, sizeof(Article));

  article->slug = calloc(1, strlen(slug) + 1);
  strcpy(article->slug, slug);

  article->title = calloc(1, strlen(title) + 1);
  strcpy(article->title, title);

  article->description = calloc(1, strlen(description) + 1);
  strcpy(article->description, description);

  return article;
};

void free_article(Article *article) {
  if (article == NULL)
    return;

  if (article->slug)
    free(article->slug);

  if (article->title)
    free(article->title);

  if (article->description)
    free(article->description);

  free(article);
}
