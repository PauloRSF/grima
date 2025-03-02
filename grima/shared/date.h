#ifndef GRIMA_SHARED_DATE_H
#define GRIMA_SHARED_DATE_H

#define ISO_TIME_LENGTH 25

// Data type to hold the unix epoch timestamp in milliseconds
typedef unsigned long long epoch_ms_t;

epoch_ms_t current_unix_timestamp();

char* unix_timestamp_to_iso8601(epoch_ms_t timestamp);

#endif
