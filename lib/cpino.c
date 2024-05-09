#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include <cJSON.h>

#include "cpino.h"

static const char *log_level_strings[] = {"TRACE", "DEBUG", "INFO",
                                          "WARN",  "ERROR", "FATAL"};

static const unsigned short int log_levels[] = {10, 20, 30, 40, 50, 60};

unsigned long get_log_timestamp() {
  struct timespec current_time_spec;

  clock_gettime(CLOCK_REALTIME, &current_time_spec);

  unsigned long current_time_millis = current_time_spec.tv_sec * 1000000;

  current_time_millis += current_time_spec.tv_nsec / 1000;

  if (current_time_spec.tv_nsec % 1000 >= 500) {
    ++current_time_millis;
  }

  return current_time_millis / 1000;
}

void cpino_log_with_context(unsigned short int level, cJSON *context, const char *fmt, ...) {
  cJSON *log_json = cJSON_CreateObject();

  cJSON_AddStringToObject(log_json, "level", log_level_strings[level]);

  cJSON_AddNumberToObject(log_json, "time", get_log_timestamp());

  cJSON_AddNumberToObject(log_json, "pid", getpid());

  // I need to iterate over the arguments twice because i need to calculate
  // the message size beforehand
  va_list arg_list;
  va_list arg_list_copy;

  va_copy(arg_list_copy, arg_list);

  va_start(arg_list, fmt);
  int message_size = vsnprintf(NULL, 0, fmt, arg_list);
  va_end(arg_list);

  char *message = malloc(message_size + 1);

  va_start(arg_list_copy, fmt);
  vsprintf(message, fmt, arg_list_copy);
  va_end(arg_list_copy);

  cJSON_AddStringToObject(log_json, "msg", message);

  free(message);

  char* default_log_string = cJSON_PrintUnformatted(log_json);

  if(context) {
    char* context_string = cJSON_PrintUnformatted(context);

    char* log_string_with_context = calloc(1, strlen(default_log_string) + strlen(context_string));

    // Disgusting way of combining two JSONs
    strncpy(log_string_with_context, default_log_string, strlen(default_log_string) - 1);
    strcat(log_string_with_context, ",");
    strncat(log_string_with_context, context_string + 1, strlen(context_string));

    printf("%s\n", log_string_with_context);

    free(context_string);
    free(log_string_with_context);
  } else {
    printf("%s\n", default_log_string);
  }

  cJSON_Delete(log_json);
  free(default_log_string);
}
