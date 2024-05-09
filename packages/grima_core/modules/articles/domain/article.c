#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "../../../shared/dates/date.h"
#include "../articles.h"

char *slugify(char *text) {
  char *slug = malloc(strlen(text) + 1);

  uint slug_index = 0;

  for (int i = 0; i < strlen(text); ++i) {
    if (isspace(text[i]))
      slug[slug_index++] = '-';
    else if (isalnum(text[i]))
      slug[slug_index++] = tolower(text[i]);
  }

  slug[slug_index] = '\0';

  return realloc(slug, slug_index);
}

Article *create_article(char *title, char *description) {
  Article *article = (Article *)malloc(sizeof(Article));

  article->slug = slugify(title);

  article->title = malloc(strlen(title) + 1);
  strcpy(article->title, title);

  article->description = malloc(strlen(description) + 1);
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

void free_find_articles_result(FindArticlesResult result) {
  for (int i = 0; i < result.count; ++i) {
    free_article(result.articles[i]);
  }

  free(result.articles);
}
