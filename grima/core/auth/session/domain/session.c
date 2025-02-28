#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <shared/strings.h>

#include "../account.h"

#define SESSION_EXPIRATION_TIME_IN_MS 1800000

struct session_create_result Session_create(account_id_t account_id) {
  bool account_exists = AccountRepository_exists_by_id(account_id);

  if (!account_exists) {
    struct session_create_result result = {
        .success = false,
    };

    result.value.error = SESSION_ACCOUNT_NOT_FOUND;

    return result;
  }

  struct session *session = malloc(sizeof(struct session));

  SessionRepository_get_next_id(session->id);

  memcpy(session->account_id, account_id, 16);

  epoch_ms_t now = current_unix_timestamp();
  session->created_at = now;
  session->expires_at = now + SESSION_EXPIRATION_TIME_IN_MS;

  struct session_create_result result = {
    .success = true,
  };

  result.value.success = session;

  return result;
}

#ifdef DEV
#include <stdio.h>

void Session_pretty_print(struct session *session) {
  char *created_at = calloc(ISO_TIME_LENGTH, sizeof(char));
  unix_timestamp_to_iso8601(session->created_at, created_at);

  char *expires_at = calloc(ISO_TIME_LENGTH, sizeof(char));
  unix_timestamp_to_iso8601(session->expires_at, expires_at);

  printf(
    "Session:\n{\n\tID: %s,\n\taccount_id: %s,\n\tcreated_at: %s,\n\texpires_at: %s\n\t}\n",
    session->id,
    session->account_id,
    created_at,
    expires_at
  );

  free(created_at);
  free(expires_at);
}
#endif
