#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <uuid/uuid.h>

#include "../account.h"

typedef struct validation_result {
  bool is_valid;
  char *error_message;
} ValidationResult;

ValidationResult validate_email(char *email) {
  ValidationResult result = {.is_valid = true, .error_message = NULL};

  if (email == NULL) {
    result.is_valid = false;
    result.error_message = "is required";

    return result;
  }

  if (strcmp(email, "") == 0) {
    result.is_valid = false;
    result.error_message = "can't be blank";

    return result;
  }

  if (strlen(email) < 4) {
    result.is_valid = false;
    result.error_message = "must be at least 4 characters";

    return result;
  }

  if (strlen(email) > 255) {
    result.is_valid = false;
    result.error_message = "must be at most 255 characters";

    return result;
  }

  for (size_t i = 0; i < strlen(email); i++) {
    if (email[i] <= 32 || email[i] >= 127) {
      result.is_valid = false;
      result.error_message = "must only contain letters, numbers or underscores";

      return result;
    };
  }

  if (strchr(email, '@') == NULL || strchr(email, '.') == NULL) {
    result.is_valid = false;
    result.error_message = "must be a valid email address";

    return result;
  }

  return result;
}

ValidationResult validate_username(char *username) {
  ValidationResult result = {.is_valid = true, .error_message = NULL};

  if (username == NULL) {
    result.is_valid = false;
    result.error_message = "is required";

    return result;
  }

  if (strcmp(username, "") == 0) {
    result.is_valid = false;
    result.error_message = "can't be blank";

    return result;
  }

  if (strlen(username) < 4) {
    result.is_valid = false;
    result.error_message = "must be at least 4 characters";

    return result;
  }

  if (strlen(username) > 32) {
    result.is_valid = false;
    result.error_message = "must be at most 32 characters";

    return result;
  }

  for (size_t i = 0; i < strlen(username); i++) {
    if (!isalnum(username[i]) && username[i] != '_') {
      result.is_valid = false;
      result.error_message = "must only contain letters, numbers or underscores";

      return result;
    };
  }

  return result;
}

typedef struct password_validation_result {
  bool is_valid;
  size_t errors_count;
  char **error_messages;
} PasswordValidationResult;

PasswordValidationResult validate_password(char *password) {
  PasswordValidationResult result = {.is_valid = true, .errors_count = 0, .error_messages = NULL};

  if (password == NULL) {
    result.is_valid = false;

    result.errors_count++;
    result.error_messages = realloc(result.error_messages, result.errors_count * sizeof(char *));
    result.error_messages[result.errors_count - 1] = "is required";

    return result;
  }

  if (strcmp(password, "") == 0) {
    result.is_valid = false;

    result.errors_count++;
    result.error_messages = realloc(result.error_messages, result.errors_count * sizeof(char *));
    result.error_messages[result.errors_count - 1] = "can't be blank";

    return result;
  }

  if (strlen(password) < 8) {
    result.is_valid = false;

    result.errors_count++;
    result.error_messages = realloc(result.error_messages, result.errors_count * sizeof(char *));
    result.error_messages[result.errors_count - 1] = "must be at least 8 characters";
  }

  if (strlen(password) > 255) {
    result.is_valid = false;

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
    result.is_valid = false;

    result.errors_count++;
    result.error_messages = realloc(result.error_messages, result.errors_count * sizeof(char *));
    result.error_messages[result.errors_count - 1] = "must contain at least one uppercase letter";
  }

  if (!contains_lowercase) {
    result.is_valid = false;

    result.errors_count++;
    result.error_messages = realloc(result.error_messages, result.errors_count * sizeof(char *));
    result.error_messages[result.errors_count - 1] = "must contain at least one lowercase letter";
  }

  if (!contains_number) {
    result.is_valid = false;

    result.errors_count++;
    result.error_messages = realloc(result.error_messages, result.errors_count * sizeof(char *));
    result.error_messages[result.errors_count - 1] = "must contain at least one number";
  }

  if (!contains_special) {
    result.is_valid = false;

    result.errors_count++;
    result.error_messages = realloc(result.error_messages, result.errors_count * sizeof(char *));
    result.error_messages[result.errors_count - 1] = "must contain at least one special character";
  }

  return result;
}

AccountValidationResult Account_validate_data(char *email, char *username, char *password) {
  size_t errors_count = 0;
  AccountValidationError *errors = NULL;

  ValidationResult email_validation_result = validate_email(email);

  if (!email_validation_result.is_valid) {
    AccountValidationError email_error;

    email_error.key = "email";
    email_error.message = email_validation_result.error_message;

    errors_count++;
    errors = realloc(errors, errors_count * sizeof(AccountValidationError));
    errors[errors_count - 1] = email_error;
  }

  ValidationResult username_validation_result = validate_username(username);

  if (!username_validation_result.is_valid) {
    AccountValidationError username_error;

    username_error.key = "username";
    username_error.message = username_validation_result.error_message;

    errors_count++;
    errors = realloc(errors, errors_count * sizeof(AccountValidationError));
    errors[errors_count - 1] = username_error;
  }

  PasswordValidationResult password_validation_result = validate_password(password);

  if (!password_validation_result.is_valid) {
    for (size_t i = 0; i < password_validation_result.errors_count; i++) {
      AccountValidationError password_error;

      password_error.key = "password";
      password_error.message = password_validation_result.error_messages[i];

      errors_count++;
      errors = realloc(errors, errors_count * sizeof(AccountValidationError));
      errors[errors_count - 1] = password_error;
    }
  }

  AccountValidationResult result;

  result.errors_count = errors_count;

  if (errors_count > 0)
    result.errors = errors;

  return result;
}
