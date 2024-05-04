#include <cJSON.h>

#include <grima_core.h>

cJSON *ArticleJSONMapper_to_json(Article *article) {
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

char *FindArticleResultJSONMapper_to_json(FindArticlesResult result) {
  cJSON *json = cJSON_CreateObject();

  if (cJSON_AddNumberToObject(json, "count", result.count) == NULL) {
    cJSON_Delete(json);
    return NULL;
  };

  cJSON *articles = cJSON_CreateArray();

  for (int i = 0; i < result.count; i++) {
    cJSON *article = cJSON_CreateObject();

    if (cJSON_AddStringToObject(json, "slug", result.articles[i]->slug) == NULL) {
      cJSON_Delete(json);
      cJSON_Delete(article);
      cJSON_Delete(articles);
      return NULL;
    };

    if (cJSON_AddStringToObject(json, "title", result.articles[i]->title) == NULL) {
      cJSON_Delete(json);
      cJSON_Delete(article);
      cJSON_Delete(articles);
      return NULL;
    };

    if (cJSON_AddStringToObject(json, "description", result.articles[i]->description) == NULL) {
      cJSON_Delete(json);
      cJSON_Delete(article);
      cJSON_Delete(articles);
      return NULL;
    };

    cJSON_AddItemToArray(articles, article);
  }

  char* json_string = cJSON_Print(json);

  cJSON_Delete(json);

  return json_string;
}