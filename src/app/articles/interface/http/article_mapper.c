#include <cJSON.h>

#include <grima/articles.h>

cJSON *ArticleJSONMapper_to_json_object(Article *article) {
  cJSON *json = cJSON_CreateObject();

  if (cJSON_AddStringToObject(json, "slug", article->slug) == NULL) {
    cJSON_Delete(json);
    return NULL;
  };

  if (cJSON_AddStringToObject(json, "title", article->title) == NULL) {
    cJSON_Delete(json);
    return NULL;
  };

  if (cJSON_AddStringToObject(json, "description", article->description) == NULL) {
    cJSON_Delete(json);
    return NULL;
  };

  return json;
}

cJSON *FindArticleResultJSONMapper_to_json_object(FindArticlesResult result) {
  cJSON *json = cJSON_CreateObject();

  if (cJSON_AddNumberToObject(json, "articlesCount", result.count) == NULL) {
    cJSON_Delete(json);
    return NULL;
  };

  cJSON *articles = cJSON_AddArrayToObject(json, "articles");

  for (int i = 0; i < result.count; i++) {
    cJSON *article = ArticleJSONMapper_to_json_object(result.articles[i]);

    if (article == NULL) {
      cJSON_Delete(json);
      cJSON_Delete(article);
      return NULL;
    };

    cJSON_AddItemToArray(articles, article);
  }

  return json;
}

char *FindArticleResultJSONMapper_to_json(FindArticlesResult result) {
  cJSON *json = FindArticleResultJSONMapper_to_json_object(result);

  char* json_string = cJSON_Print(json);

  cJSON_Delete(json);

  return json_string;
}
