#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <grima/dates.h>

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

const int number_of_days_in_month[12] = {31, 28, 31, 30, 31, 30,
                                         31, 31, 30, 31, 30, 31};

bool is_well_formatted_date_string(char *date_string) {
  if (date_string == NULL || strlen(date_string) != 10)
    return false;

  int year, month, day;

  int result = sscanf(date_string, "%4d-%2d-%2d", &year, &month, &day);

  return result == 3;
}

bool is_valid_date_string(char *date_string) {
  if (!is_well_formatted_date_string(date_string))
    return false;

  int year, month, day;

  sscanf(date_string, "%4d-%2d-%2d", &year, &month, &day);

  if (month < 1 && month > 12)
    return false;

  if (day < 1)
    return false;

  bool is_leap_year = year % 4 == 0 && year % 100 != 0;

  if (is_leap_year && month == 2 && day > 29)
    return false;

  if (day > number_of_days_in_month[month])
    return false;

  return true;
}