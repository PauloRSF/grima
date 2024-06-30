#include <stdbool.h>

#include <sodium.h>

bool setup_crypto() { return sodium_init() >= 0; }
