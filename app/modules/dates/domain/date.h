#ifndef GRIMA_DATES_DOMAIN_DATE_H
#define GRIMA_DATES_DOMAIN_DATE_H

typedef struct date {
  unsigned short day;
  unsigned short month;
  unsigned short year;
} Date;

char *date_to_string(Date date);

Date date_from_string(char *date_string);

#endif