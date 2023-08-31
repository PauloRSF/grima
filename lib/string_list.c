#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "string_list.h"

StringList *StringList_new() {
  StringList *list = calloc(sizeof(StringList), 1);

  list->items = NULL;

  return list;
}

void StringList_add(StringList *list, char *item) {
  if (list == NULL || item == NULL)
    return;

  StringListNode *new_node = calloc(sizeof(StringListNode), 1);
  new_node->data = item;
  new_node->next = NULL;

  if (list->items == NULL) {
    list->items = new_node;
  } else {
    StringListNode *node = list->items;

    while (node->next != NULL) {
      node = node->next;
    }

    node->next = new_node;
  }
};

StringList *StringList_clone(StringList *list) {
  if (list == NULL)
    return NULL;

  StringList *new_list = StringList_new();

  if (list->items == NULL)
    return new_list;

  new_list->items = calloc(sizeof(StringListNode), 1);

  new_list->items->next = NULL;
  new_list->items->data = malloc(strlen(list->items->data) + 1);
  strcpy((char *)new_list->items->data, list->items->data);

  StringListNode *previous_node = new_list->items;

  for (StringListNode *node = list->items->next; node != NULL;
       node = node->next) {
    StringListNode *new_node = calloc(sizeof(StringListNode), 1);

    new_node->next = NULL;
    new_node->data = malloc(strlen(node->data) + 1);
    strcpy((char *)new_node->data, node->data);

    previous_node->next = new_node;

    previous_node = new_node;
  }

  return new_list;
}

bool StringList_is_empty(StringList *list) {
  return list == NULL || list->items == NULL;
}

void StringList_free(StringList *list) {
  if (list == NULL)
    return;

  if (list->items != NULL) {
    StringListNode *node = list->items;
    StringListNode *next;

    while (node != NULL) {
      next = node->next;

      free((char *)node->data);
      free(node);

      node = next;
    }
  }

  free(list);
}
