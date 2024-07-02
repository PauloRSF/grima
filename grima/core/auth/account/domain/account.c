#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <shared/strings.h>

#include "../account.h"

struct create_account_result Account_create(char *email, char *username, char *password) {
  struct create_account_result result;

  char *clean_email = trim_whitespace(email);
  char *clean_username = trim_whitespace(username);
  char *clean_password = trim_whitespace(password);

  struct account_validation_result validation_result =
      Account_validate_data(clean_email, clean_username, clean_password);

  if (validation_result.errors_count > 0) {
    result.is_valid = false;
    result.value.errors = validation_result.errors;

    return result;
  }

  struct account *account = malloc(sizeof(struct account));

  AccountRepository_get_next_id(account->id);

  account->email = clean_email;
  account->username = clean_username;

  Account_hash_password(clean_password, &account->password);
  free(clean_password);

  result.is_valid = true;
  result.value.account = account;

  return result;
}

#ifdef DEV
#include <stdio.h>

#include <uuid/uuid.h>

void Account_pretty_print(struct account *account) {
  char id_str[37];
  uuid_unparse(account->id, id_str);

  // 1 byte = 2 chars
  char password_salt_str[ACCOUNT_PASSWORD_SALT_SIZE * 2 + 1] = {0};
  for (size_t i = 0; i < ACCOUNT_PASSWORD_SALT_SIZE; i++)
    snprintf((char *)&password_salt_str[i * 2], 3, "%02x", account->password.salt[i]);

  char password_hash_str[ACCOUNT_PASSWORD_HASH_SIZE * 2 + 1] = {0};
  for (size_t i = 0; i < ACCOUNT_PASSWORD_HASH_SIZE; i++)
    snprintf((char *)&password_hash_str[i * 2], 3, "%02x", account->password.hash[i]);


  printf("Account {\n\tID = \"%s\",\n\temail = \"%s\",\n\tusername = "
         "\"%s\",\n\tpassword = {\n\t\tsalt = \"%s\"\n\t\thash = \"%s\"\n\t}\n}\n",
         id_str, account->email, account->username, password_salt_str, password_hash_str);
}

#endif
