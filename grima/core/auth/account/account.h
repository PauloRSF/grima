#ifndef GRIMA_AUTH_ACCOUNT_H
#define GRIMA_AUTH_ACCOUNT_H

#include <stdbool.h>
#include <stdint.h>

#include <uuid/uuid.h>

// Size in bytes
#define PASSWORD_HASH_SIZE 32
#define PASSWORD_SALT_SIZE 16

// ----- Account Entity -----

typedef uuid_t AccountId;

typedef struct account {
  AccountId id;
  char *email;
  char *username;
  struct account_password {
    uint8_t *salt;
    uint8_t *hash;
  } password;
} Account;

void free_account(Account *account);

// ----- Account Validation -----

typedef struct account_validation_error {
  char *key;
  char *message;
} AccountValidationError;

typedef struct account_validation_result {
  size_t errors_count;
  AccountValidationError *errors;
} AccountValidationResult;

AccountValidationResult Account_validate_data(char *email, char *username,
                                              char *password);

// ----- Account Creation -----

typedef struct create_account_result {
  bool is_valid;
  union {
    Account *account;
    AccountValidationError *errors;
  } value;
} CreateAccountResult;

CreateAccountResult Account_create(char *email, char *username, char *password);

uint8_t *Account_hash_password(char *password, uint8_t *salt);

uint8_t *Account_generate_salt();

// ----- Account Repository -----

void AccountRepository_get_next_id(AccountId account_id);

void AccountRepository_save(Account *account);

#ifdef DEV
// ----- Development utils -----

void Account_pretty_print(Account *account);
#endif

#endif
