#include <grima/articles.h>

FindArticlesResult find_articles(FindArticlesOptions options) {
  FindArticlesResult result = ArticleRepository_find_articles(options);

  return result;
};
