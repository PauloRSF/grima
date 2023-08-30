#ifndef GRIMA_DATES_DOMAIN_DATE_H
#define GRIMA_DATES_DOMAIN_DATE_H

#include <stdbool.h>

typedef struct date {
  unsigned short day;
  unsigned short month;
  unsigned short year;
} Date;

char *date_to_string(Date date);

Date date_from_string(char *date_string);

bool is_well_formatted_date_string(char *date_string);

bool is_valid_date_string(char *date_string);

#endif