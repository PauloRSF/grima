#ifndef GRIMA_BLOG_COMMENT_H
#define GRIMA_BLOG_COMMENT_H

#include <stdbool.h>
#include <stdint.h>

#include <uuid/uuid.h>
#include <hashmap.h>

#include <shared/date.h>
#include <shared/result.h>
#include <shared/errors.h>

// ----- Comment Entity -----

struct comment {
  char *id;
  char *author_id;
  char *article_id;
  char *body;
  epoch_ms_t created_at;
  epoch_ms_t updated_at;
};

void Comment_free(struct comment *comment);

// ----- Comment Creation -----

RESULT_STRUCT(create_comment_result, struct comment *, ValidationErrors);

struct create_comment_result Comment_create(char *article_id, char *author_id, char *body);

// ----- Comment Repository -----

char *CommentRepository_get_next_id();

enum comment_repository_save_error {
  COMMENT_REPOSITORY_SAVE_AUTHOR_NOT_FOUND,
  COMMENT_REPOSITORY_SAVE_ARTICLE_NOT_FOUND,
  COMMENT_REPOSITORY_SAVE_APPLICATION_ERROR,
};

RESULT_STRUCT(comment_repository_save_result, void *, enum comment_repository_save_error);

struct comment_repository_save_result CommentRepository_save(struct comment *comment);

#ifdef DEV

void Comment_show(struct comment *comment);
void Comment_show_creation_result(struct create_comment_result result);

#endif

#endif
