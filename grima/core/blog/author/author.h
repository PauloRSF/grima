#ifndef GRIMA_BLOG_AUTHOR_H
#define GRIMA_BLOG_AUTHOR_H

#include <stdbool.h>
#include <stdint.h>

#include <uuid/uuid.h>

#include <shared/date.h>
#include <shared/result.h>
#include <shared/errors.h>

#include <hashmap.h>

// ----- Author Entity -----

struct author {
  char *id;
  char *username;
  char *bio;
  char *image;
  epoch_ms_t created_at;
  epoch_ms_t updated_at;
};

void Author_free(struct author *author);

// ----- Author Creation -----

RESULT_STRUCT(create_author_result, struct author *, ValidationErrors);

struct create_author_result Author_create(char *username, char *bio, char *image);

// ----- Author Repository -----

char *AuthorRepository_get_next_id();

enum author_repository_save_error {
  AUTHOR_REPOSITORY_SAVE_USERNAME_ALREADY_TAKEN,
  AUTHOR_REPOSITORY_SAVE_APPLICATION_ERROR,
};

RESULT_STRUCT(author_repository_save_result, void *, enum author_repository_save_error);

struct author_repository_save_result AuthorRepository_save(struct author *author);

RESULT_STRUCT(author_repository_exists_by_id_result, bool, void *);

struct author_repository_exists_by_id_result AuthorRepository_exists_by_id(char *author_id);

#ifdef DEV

void Author_show(struct author *author);
void Author_show_creation_result(struct create_author_result result);

#endif

#endif
