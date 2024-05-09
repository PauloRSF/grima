#include <stdlib.h>

#include <grima_core.h>

FindArticlesResult find_articles() {
  FindArticlesResult result = {
    .count = 0,
    .articles = NULL
  };

  result.count = 1;
  result.articles = malloc(sizeof(Article));

  Article *article = create_article("A blog post", "This is a blog post written by me");

  result.articles[0] = article;

  return result;
}
