#ifndef GRIMA_BLOG_AUTHOR_H
#define GRIMA_BLOG_AUTHOR_H

#include <stdbool.h>
#include <stdint.h>

#include <uuid/uuid.h>

#include <shared/date.h>
#include <shared/result.h>

#include <hashmap.h>

// ----- Author Entity -----

typedef char author_id_t[37];

struct author {
  author_id_t id;
  char *username;
  char *bio;
  char *image;
  epoch_ms_t created_at;
  epoch_ms_t updated_at;
};

void Author_free(struct author *author);

// ----- Author Creation -----

enum author_creation_error_kind {
  AUTHOR_VALIDATION_ERROR,
};

struct author_validation_error {
  char *field;
  char **errors;
};

typedef struct hashmap *author_validation_errors;

struct author_creation_error {
  enum author_creation_error_kind kind;
  union {
    author_validation_errors validation_errors;
  } error;
};

RESULT_STRUCT(create_author_result, struct author *, struct author_creation_error);

struct create_author_result Author_create(char *username, char *bio, char *image);

// ----- Author Repository -----

char *AuthorRepository_get_next_id();

enum author_repository_save_error {
  AUTHOR_REPOSITORY_SAVE_USERNAME_ALREADY_TAKEN,
  AUTHOR_REPOSITORY_SAVE_APPLICATION_ERROR,
};

RESULT_STRUCT(author_repository_save_result, void *, enum author_repository_save_error);

struct author_repository_save_result AuthorRepository_save(struct author *author);

#ifdef DEV

void Author_show(struct author *author);
void Author_show_creation_error(struct author_creation_error creation_error);

#endif

#endif
