#include <stdlib.h>
#include <string.h>

#include <argon2.h>
#include <sodium.h>

#include <grima/auth.h>

void Account_hash_password(char *plaintext_password, struct account_password *password) {
  randombytes_buf(&password->salt, ACCOUNT_PASSWORD_SALT_SIZE);

  argon2i_hash_raw(2, 1 << 16, 1, plaintext_password, strlen(plaintext_password), password->salt,
                   ACCOUNT_PASSWORD_SALT_SIZE, &password->hash, ACCOUNT_PASSWORD_HASH_SIZE);
}
