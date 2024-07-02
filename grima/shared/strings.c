#include <ctype.h>
#include <stdlib.h>
#include <string.h>

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
