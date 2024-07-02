#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

#include "date.h"

epoch_ms_t current_unix_timestamp() {
  struct timespec ts;
  timespec_get(&ts, TIME_UTC);

  return ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

void unix_timestamp_to_iso8601(epoch_ms_t timestamp, char *buffer) {
  time_t time = timestamp / 1000;
  struct tm tm;
  gmtime_r(&time, &tm);

  strftime(buffer, ISO_TIME_LENGTH, "%FT%T", &tm);

  char millis[6];
  snprintf(millis, 6, ".%03lu", timestamp % 1000);

  strcat(buffer, millis);
  strcat(buffer, "Z");
}
