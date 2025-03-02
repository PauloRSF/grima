#include <errno.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <cpino.h>

#include <grima/app.h>
#include <grima/blog/author.h>
#include <grima/blog/article.h>
#include <grima/blog/comment.h>

#include "./database/database.h"

#define DATABASE_MODULE_INDEX 0

bool modules_to_shutdown[1] = {false};

void mark_module_as_initialized(uint module_index) { modules_to_shutdown[module_index] = true; }

AppContext ctx;

bool is_dying = false;

void shutdown_handler(int signal) {
  if (signal == SIGSEGV) {
    cpino_log_fatal("Shutting down due to segfault...");
  } else {
    cpino_log_info("Shutting down...");
  }

  if (is_dying) {
    cpino_log_fatal("Killing application due to previous shutdown signal");
    exit(signal);
  }

  is_dying = true;

  shutdown_app(ctx);

  exit(signal);
}

void start_app(AppContext *app_ctx) {
  setvbuf(stdout, NULL, _IONBF, 0);

  signal(SIGINT, shutdown_handler);
  signal(SIGSEGV, shutdown_handler);

  cpino_log_info("[BOOT] Booting application");

  cpino_log_info("[BOOT] Initializing database");
  bool was_database_setup_successful = connect_database("postgres://postgres:postgres@postgres:5432/grima_development");

  if (was_database_setup_successful) {
    cpino_log_info("[BOOT] Database initialized");
  } else {
    cpino_log_fatal("[BOOT] Failed to setup the database. Exiting...");
    return shutdown_app(*app_ctx);
  };
}

void shutdown_app(AppContext app_ctx) { cpino_log_info("[BOOT] Shutting down application"); };

int main() {
  start_app(&ctx);

  struct create_author_result author_creation_result = Author_create("foobar", "", "");
  Author_show_creation_result(author_creation_result);
  if (!author_creation_result.success) return 1;

  struct author *author = author_creation_result.value.success;

  struct author_repository_save_result author_save_result = AuthorRepository_save(author);
  printf("AuthorRepository_save success: %s\n", author_save_result.success ? "true" : "false");
  if (!author_save_result.success) return 1;

  struct create_article_result article_creation_result =
      Article_create(author->id, "Hello, World!", "This is a test article", "This is the body of the test article");
  Article_show_creation_result(article_creation_result);
  if (!article_creation_result.success) return 1;

  struct article *article = article_creation_result.value.success;

  struct article_repository_save_result article_save_result = ArticleRepository_save(article);
  printf("ArticleRepository_save success: %s\n", article_save_result.success ? "true" : "false");
  if (!article_save_result.success) return 1;

  struct create_comment_result comment_creation_result = Comment_create(author->id, article->id, "This is a test comment");
  Comment_show_creation_result(comment_creation_result);
  if (!comment_creation_result.success) return 1;

  struct comment *comment = comment_creation_result.value.success;

  struct comment_repository_save_result comment_save_result = CommentRepository_save(comment);
  printf("CommentRepository_save success: %s\n", comment_save_result.success ? "true" : "false");
  if (!comment_save_result.success) return 1;

  return 0;
}
