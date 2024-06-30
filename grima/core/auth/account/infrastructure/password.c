#include <stdlib.h>
#include <string.h>

#include <argon2.h>
#include <sodium.h>

#include <grima/auth.h>

uint8_t *Account_hash_password(char *password, uint8_t *salt) {
  uint8_t *hash = calloc(PASSWORD_HASH_SIZE, sizeof(uint8_t));

  argon2i_hash_raw(2, 1 << 16, 1, password, strlen(password), salt, PASSWORD_SALT_SIZE, hash,
                   PASSWORD_HASH_SIZE);

  return hash;
}

uint8_t *Account_generate_salt() {
  uint8_t *salt = calloc(PASSWORD_SALT_SIZE, sizeof(uint8_t));

  randombytes_buf(salt, PASSWORD_SALT_SIZE);

  return salt;
}
