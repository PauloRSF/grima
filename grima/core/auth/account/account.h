#ifndef GRIMA_AUTH_ACCOUNT_H
#define GRIMA_AUTH_ACCOUNT_H

#include <stdbool.h>
#include <stdint.h>

#include <shared/date.h>

#include <uuid/uuid.h>

// Size in bytes
#define ACCOUNT_PASSWORD_HASH_SIZE 32
#define ACCOUNT_PASSWORD_SALT_SIZE 16

// ----- Account Entity -----

typedef uuid_t account_id_t;

struct account_password {
  uint8_t salt[ACCOUNT_PASSWORD_SALT_SIZE];
  uint8_t hash[ACCOUNT_PASSWORD_HASH_SIZE];
};

struct account {
  account_id_t id;
  char *email;
  char *username;
  struct account_password password;
  epoch_ms_t created_at;
  epoch_ms_t updated_at;
};

void free_account(struct account *account);

// ----- Account Validation -----

struct account_validation_error {
  char *key;
  char *message;
};

struct account_validation_result {
  size_t errors_count;
  struct account_validation_error *errors;
};

struct account_validation_result
Account_validate_data(char *email, char *username, char *password);

// ----- Account Creation -----

struct create_account_result {
  bool is_valid;
  union {
    struct account *account;
    struct account_validation_error *errors;
  } value;
};

struct create_account_result Account_create(char *email, char *username,
                                            char *password);

void Account_hash_password(char *plaintext_password,
                           struct account_password *password);

// ----- Account Repository -----

void AccountRepository_get_next_id(account_id_t account_id);

enum account_repository_save_result {
  ACCOUNT_REPOSITORY_SAVE_SUCCESS,
  ACCOUNT_REPOSITORY_SAVE_EMAIL_ALREADY_TAKEN,
  ACCOUNT_REPOSITORY_SAVE_USERNAME_ALREADY_TAKEN,
  ACCOUNT_REPOSITORY_SAVE_APPLICATION_ERROR,
};

enum account_repository_save_result
AccountRepository_save(struct account *account);

#ifdef DEV
// ----- Development utils -----

void Account_pretty_print(struct account *account);
#endif

#endif
