#ifndef GRIMA_BLOG_FAVORITE_H
#define GRIMA_BLOG_FAVORITE_H

#include <stdbool.h>

#include <shared/date.h>
#include <shared/result.h>
#include <shared/errors.h>

// ----- Favorite Entity -----

struct favorite {
  entity_id_t id;
  entity_id_t author_id;
  entity_id_t article_id;
  epoch_ms_t created_at;
};

void Favorite_free(struct favorite *favorite);

// ----- Favorite Creation -----

RESULT_STRUCT(create_favorite_result, struct favorite *, ValidationErrors);

struct create_favorite_result Favorite_create(entity_id_t article_id, entity_id_t author_id);

// ----- Favorite Repository -----

char *FavoriteRepository_get_next_id();

enum favorite_repository_save_error {
  FAVORITE_REPOSITORY_SAVE_APPLICATION_ERROR,
  FAVORITE_REPOSITORY_SAVE_AUTHOR_NOT_FOUND,
  FAVORITE_REPOSITORY_SAVE_ARTICLE_NOT_FOUND,
};

RESULT_STRUCT(favorite_repository_save_result, void *, enum favorite_repository_save_error);

struct favorite_repository_save_result FavoriteRepository_save(struct favorite *favorite);

RESULT_STRUCT(favorite_repository_exists_result, bool, void *);

struct favorite_repository_exists_result FavoriteRepository_exists(entity_id_t author_id, entity_id_t article_id);

#ifdef DEV

void Favorite_show(struct favorite *favorite);
void Favorite_show_creation_result(struct create_favorite_result result);

#endif

#endif
