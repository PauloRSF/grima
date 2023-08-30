#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cJSON.h>

#include "date.h"

char *date_to_string(Date date) {
  char *date_of_birth = calloc(1, 11);

  sprintf(date_of_birth, "%d-%02d-%02d", date.year, date.month, date.day);

  return date_of_birth;
}

Date date_from_string(char *date_string) {
  char date_of_birth_year[5] = {'\0'};
  char date_of_birth_month[3] = {'\0'};
  char date_of_birth_day[3] = {'\0'};

  strncpy(date_of_birth_year, date_string, 4);
  strncpy(date_of_birth_month, date_string + 5, 2);
  strncpy(date_of_birth_day, date_string + 8, 2);

  return (Date){.day = atoi(date_of_birth_day),
                .month = atoi(date_of_birth_month),
                .year = atoi(date_of_birth_year)};
}