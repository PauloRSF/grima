#ifndef GRIMA_BLOG_ARTICLE_H
#define GRIMA_BLOG_ARTICLE_H

#include <stdbool.h>

#include <hashmap.h>

#include <shared/date.h>
#include <shared/result.h>
#include <shared/errors.h>

// ----- Article Entity -----

struct article {
  entity_id_t id;
  entity_id_t author_id;
  char *slug;
  char *title;
  char *description;
  char *body;
  epoch_ms_t created_at;
  epoch_ms_t updated_at;
};

void Article_free(struct article *article);

// ----- Article Creation -----

RESULT_STRUCT(create_article_result, struct article *, ValidationErrors);

struct create_article_result Article_create(entity_id_t author_id, char *title, char *description, char *body);

// ----- Article Repository -----

char *ArticleRepository_get_next_id();

enum article_repository_save_error {
  ARTICLE_REPOSITORY_SAVE_APPLICATION_ERROR,
  ARTICLE_REPOSITORY_SAVE_AUTHOR_NOT_FOUND,
  ARTICLE_REPOSITORY_SAVE_SLUG_ALREADY_TAKEN,
};

RESULT_STRUCT(article_repository_save_result, void *, enum article_repository_save_error);

struct article_repository_save_result ArticleRepository_save(struct article *article);

RESULT_STRUCT(article_repository_exists_by_id_result, bool, void *);

struct article_repository_exists_by_id_result ArticleRepository_exists_by_id(entity_id_t article_id);

#ifdef DEV

void Article_show(struct article *article);
void Article_show_creation_result(struct create_article_result result);

#endif

#endif
