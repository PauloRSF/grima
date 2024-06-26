#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "../account.h"

char *trim_whitespace(char *value) {
  if (value == NULL)
    return NULL;

  if (strcmp(value, "") == 0)
    return value;

  size_t start = 0;
  size_t end = strlen(value) - 1;

  while (isspace(value[start]) && start < end)
    ++start;

  while (isspace(value[end]) && end > start)
    --end;

  size_t trimmed_length = end - start + 1;

  char *trimmed = malloc(trimmed_length + 1);

  strncpy(trimmed, value + start, trimmed_length);

  trimmed[trimmed_length] = '\0';

  return trimmed;
}

CreateAccountResult Account_create(char *email, char *username, char *password) {
  CreateAccountResult result;

  char *clean_email = trim_whitespace(email);
  char *clean_username = trim_whitespace(username);
  char *clean_password = trim_whitespace(password);

  AccountValidationResult validation_result =
      Account_validate_data(clean_email, clean_username, clean_password);

  if (validation_result.errors_count > 0) {
    result.is_valid = false;
    result.value.errors = validation_result.errors;

    return result;
  }

  Account *account = malloc(sizeof(Account));

  AccountRepository_get_next_id(account->id);
  account->email = clean_email;
  account->username = clean_username;
  account->password.salt = Account_generate_salt();
  account->password.hash = Account_hash_password(clean_password, account->password.salt);
  free(clean_password);

  result.is_valid = true;
  result.value.account = account;

  return result;
}

#ifdef DEV
#include <stdio.h>

#include <uuid/uuid.h>

void Account_pretty_print(Account *account) {
  char id_str[37];
  uuid_unparse(account->id, id_str);

  // 1 byte = 2 chars
  char password_salt_str[PASSWORD_SALT_SIZE * 2 + 1] = {0};
  for (size_t i = 0; i < PASSWORD_SALT_SIZE; i++)
    sprintf((char *)&password_salt_str[i * 2], "%02x", account->password.salt[i]);

  char password_hash_str[PASSWORD_HASH_SIZE * 2 + 1] = {0};
  for (size_t i = 0; i < PASSWORD_HASH_SIZE; i++)
    sprintf((char *)&password_hash_str[i * 2], "%02x", account->password.hash[i]);

  printf("Account {\n\tID = \"%s\",\n\temail = \"%s\",\n\tusername = "
         "\"%s\",\n\tpassword = {\n\t\tsalt = \"%s\"\n\t\thash = \"%s\"\n\t}\n}\n",
         id_str, account->email, account->username, password_salt_str, password_hash_str);
}

#endif
