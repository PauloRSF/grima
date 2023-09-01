#ifndef STRING_LIST_H
#define STRING_LIST_H

#include <stdbool.h>
#include <stddef.h>

typedef struct string_list_node {
  char *data;
  struct string_list_node *next;
} StringListNode;

typedef struct string_list {
  StringListNode *items;
} StringList;

StringList *StringList_new();
void StringList_add(StringList *list, char *data);
StringList *StringList_clone(StringList *list);
bool StringList_is_empty(StringList *list);
size_t StringList_count(StringList *list);
void StringList_free(StringList *list);

#define StringList_ForEach(element, list)                                      \
  for (element = (list != NULL) ? (list)->items : NULL; element != NULL;       \
       element = element->next)

#endif
