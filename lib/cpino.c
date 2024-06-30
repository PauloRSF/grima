#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <cJSON.h>

#include "cpino.h"

static const char *log_level_strings[] = {"TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"};

static const unsigned short int log_levels[] = {10, 20, 30, 40, 50, 60};

unsigned long get_current_timestamp() {
  struct timespec ts;
  timespec_get(&ts, TIME_UTC);

  return ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

char *build_default_log_string(unsigned short int level, char *message) {
  const char *level_string = log_level_strings[level];

  unsigned long timestamp = get_current_timestamp();

  int pid = getpid();

  char *default_log_string_format = "{\"level\":\"%s\",\"time\":%lu,\"pid\":%d,\"msg\":\"%s\"}";

  int default_log_string_length =
      snprintf(NULL, 0, default_log_string_format, level_string, timestamp, 4242, message);

  char *default_log_string = malloc(default_log_string_length + 1);

  sprintf(default_log_string, default_log_string_format, level_string, timestamp, pid, message);

  return default_log_string;
}

char *concat_json_strings(char *json1, char *json2) {
  char *concatenated_json = calloc(1, strlen(json1) + strlen(json2) + 1);

  strncpy(concatenated_json, json1, strlen(json1) - 1);
  strcat(concatenated_json, ",");
  strncat(concatenated_json, json2 + 1, strlen(json2));

  return concatenated_json;
}

void cpino_log_with_context(unsigned short int level, cJSON *context, const char *fmt, ...) {
  va_list arg_list;

  va_start(arg_list, fmt);
  int message_size = vsnprintf(NULL, 0, fmt, arg_list);
  va_end(arg_list);

  char *message = malloc(message_size + 1);

  va_start(arg_list, fmt);
  vsprintf(message, fmt, arg_list);
  va_end(arg_list);

  if (message[message_size - 1] == '\n') {
    message[message_size - 1] = '\0';
  }

  char *default_log_string = build_default_log_string(level, message);

  free(message);

  if (context) {
    char *context_string = cJSON_PrintUnformatted(context);

    char *log_string_with_context = concat_json_strings(default_log_string, context_string);

    printf("%s\n", log_string_with_context);

    free(context_string);
    free(log_string_with_context);
  } else {
    printf("%s\n", default_log_string);
  }

  free(default_log_string);
}
