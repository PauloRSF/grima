#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <shared/strings.h>

#include "../account.h"

struct validation_result {
  bool success;
  char *error_message;
};

struct validation_result validate_email(char *email) {
  struct validation_result result = {.success = true, .error_message = NULL};

  if (email == NULL) {
    result.success = false;
    result.error_message = "is required";

    return result;
  }

  if (strcmp(email, "") == 0) {
    result.success = false;
    result.error_message = "can't be blank";

    return result;
  }

  if (strlen(email) < 4) {
    result.success = false;
    result.error_message = "must be at least 4 characters";

    return result;
  }

  if (strlen(email) > 255) {
    result.success = false;
    result.error_message = "must be at most 255 characters";

    return result;
  }

  for (size_t i = 0; i < strlen(email); i++) {
    if (email[i] <= 32 || email[i] >= 127) {
      result.success = false;
      result.error_message = "must only contain letters, numbers or underscores";

      return result;
    };
  }

  if (strchr(email, '@') == NULL || strchr(email, '.') == NULL) {
    result.success = false;
    result.error_message = "must be a valid email address";

    return result;
  }

  return result;
}

struct validation_result validate_username(char *username) {
  struct validation_result result = {.success = true, .error_message = NULL};

  if (username == NULL) {
    result.success = false;
    result.error_message = "is required";

    return result;
  }

  if (strcmp(username, "") == 0) {
    result.success = false;
    result.error_message = "can't be blank";

    return result;
  }

  if (strlen(username) < 4) {
    result.success = false;
    result.error_message = "must be at least 4 characters";

    return result;
  }

  if (strlen(username) > 32) {
    result.success = false;
    result.error_message = "must be at most 32 characters";

    return result;
  }

  for (size_t i = 0; i < strlen(username); i++) {
    if (!isalnum(username[i]) && username[i] != '_') {
      result.success = false;
      result.error_message = "must only contain letters, numbers or underscores";

      return result;
    };
  }

  return result;
}

struct password_validation_result {
  bool success;
  size_t errors_count;
  char **error_messages;
};

struct password_validation_result validate_password(char *password) {
  struct password_validation_result result = {
      .success = true, .errors_count = 0, .error_messages = NULL};

  if (password == NULL) {
    result.success = false;

    result.errors_count++;
    result.error_messages = realloc(result.error_messages, result.errors_count * sizeof(char *));
    result.error_messages[result.errors_count - 1] = "is required";

    return result;
  }

  if (strcmp(password, "") == 0) {
    result.success = false;

    result.errors_count++;
    result.error_messages = realloc(result.error_messages, result.errors_count * sizeof(char *));
    result.error_messages[result.errors_count - 1] = "can't be blank";

    return result;
  }

  if (strlen(password) < 8) {
    result.success = false;

    result.errors_count++;
    result.error_messages = realloc(result.error_messages, result.errors_count * sizeof(char *));
    result.error_messages[result.errors_count - 1] = "must be at least 8 characters";
  }

  if (strlen(password) > 255) {
    result.success = false;

    result.errors_count++;
    result.error_messages = realloc(result.error_messages, result.errors_count * sizeof(char *));
    result.error_messages[result.errors_count - 1] = "must be at most 255 characters";
  }

  bool contains_uppercase = false;
  bool contains_lowercase = false;
  bool contains_number = false;
  bool contains_special = false;

  for (size_t i = 0; i < strlen(password); i++) {
    if (!contains_uppercase)
      contains_uppercase = isupper(password[i]);
    if (!contains_lowercase)
      contains_lowercase = islower(password[i]);
    if (!contains_number)
      contains_number = isdigit(password[i]);
    if (!contains_special)
      contains_special = !isalnum(password[i]);
  }

  if (!contains_uppercase) {
    result.success = false;

    result.errors_count++;
    result.error_messages = realloc(result.error_messages, result.errors_count * sizeof(char *));
    result.error_messages[result.errors_count - 1] = "must contain at least one uppercase letter";
  }

  if (!contains_lowercase) {
    result.success = false;

    result.errors_count++;
    result.error_messages = realloc(result.error_messages, result.errors_count * sizeof(char *));
    result.error_messages[result.errors_count - 1] = "must contain at least one lowercase letter";
  }

  if (!contains_number) {
    result.success = false;

    result.errors_count++;
    result.error_messages = realloc(result.error_messages, result.errors_count * sizeof(char *));
    result.error_messages[result.errors_count - 1] = "must contain at least one number";
  }

  if (!contains_special) {
    result.success = false;

    result.errors_count++;
    result.error_messages = realloc(result.error_messages, result.errors_count * sizeof(char *));
    result.error_messages[result.errors_count - 1] = "must contain at least one special character";
  }

  return result;
}

struct account_validation_result Account_validate_data(char *email, char *username,
                                                       char *password) {
  size_t errors_count = 0;
  struct account_validation_error *errors = NULL;

  struct validation_result email_validation_result = validate_email(email);

  if (!email_validation_result.success) {
    struct account_validation_error email_error;

    email_error.key = "email";
    email_error.message = email_validation_result.error_message;

    errors_count++;
    errors = realloc(errors, errors_count * sizeof(struct account_validation_error));
    errors[errors_count - 1] = email_error;
  }

  struct validation_result username_validation_result = validate_username(username);

  if (!username_validation_result.success) {
    struct account_validation_error username_error;

    username_error.key = "username";
    username_error.message = username_validation_result.error_message;

    errors_count++;
    errors = realloc(errors, errors_count * sizeof(struct account_validation_error));
    errors[errors_count - 1] = username_error;
  }

  struct password_validation_result password_validation_result = validate_password(password);

  if (!password_validation_result.success) {
    for (size_t i = 0; i < password_validation_result.errors_count; i++) {
      struct account_validation_error password_error;

      password_error.key = "password";
      password_error.message = password_validation_result.error_messages[i];

      errors_count++;
      errors = realloc(errors, errors_count * sizeof(struct account_validation_error));
      errors[errors_count - 1] = password_error;
    }
  }

  struct account_validation_result result;

  result.errors_count = errors_count;

  if (errors_count > 0)
    result.errors = errors;

  return result;
}

struct create_account_result Account_create(char *email, char *username, char *password) {
  struct create_account_result result;

  char *clean_email = trim_whitespace(email);
  char *clean_username = trim_whitespace(username);
  char *clean_password = trim_whitespace(password);

  struct account_validation_result validation_result =
      Account_validate_data(clean_email, clean_username, clean_password);

  if (validation_result.errors_count > 0) {
    result.success = false;
    result.value.error = validation_result.errors;

    return result;
  }

  struct account *account = malloc(sizeof(struct account));

  AccountRepository_get_next_id(account->id);

  account->email = clean_email;
  account->username = clean_username;

  Account_hash_password(clean_password, &account->password);
  free(clean_password);

  epoch_ms_t now = current_unix_timestamp();
  account->created_at = now;
  account->updated_at = now;

  result.success = true;
  result.value.success = account;

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

  char created_at_str[ISO_TIME_LENGTH];
  unix_timestamp_to_iso8601(account->created_at, created_at_str);

  char updated_at_str[ISO_TIME_LENGTH];
  unix_timestamp_to_iso8601(account->updated_at, updated_at_str);

  printf("Account {\n\tID = \"%s\",\n\temail = \"%s\",\n\tusername = "
         "\"%s\",\n\tpassword = {\n\t\tsalt = \"%s\"\n\t\thash = \"%s\"\n\t}\n\tcreated_at = "
         "\"%s\"\n\tupdated_at = \"%s\"\n}\n",
         id_str, account->email, account->username, password_salt_str, password_hash_str,
         created_at_str, updated_at_str);
}

#endif
