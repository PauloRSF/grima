#ifndef STRING_LIST_H
#define STRING_LIST_H

#include <stdbool.h>

typedef struct string_list_node {
  const char *data;
  struct string_list_node *next;
} StringListNode;

typedef struct string_list {
  StringListNode *items;
} StringList;

StringList *StringList_new();
void StringList_add(StringList *list, const char *data);
StringList *StringList_clone(StringList *list);
bool StringList_is_empty(StringList *list);

#define StringList_ForEach(element, list)                                      \
  for (element = (list != NULL) ? (list)->items : NULL; element != NULL;       \
       element = element->next)

#endif
