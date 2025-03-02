#ifndef GRIMA_SHARED_STRINGS_H
#define GRIMA_SHARED_STRINGS_H

#include <stdbool.h>

bool is_blank_string(char *value);
char *clone_string(char *value);
char *clone_and_trim_string(char *value);

#endif
