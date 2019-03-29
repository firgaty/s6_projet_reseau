#include "dllist.h"

dllist_t* gen_dllist() {
  dllist_t* list = malloc(sizeof(dllist_t));
  list->size = 0;
  list->first = NULL;
  list->last = NULL;
  return list;
}

dllist_node_t* gen_dllist_node(void* data) {
  dllist_node_t* node = malloc(sizeof(dllist_node_t));
  node->data = data;
  return node;
}

short dllist_push_back(dllist_t* list, void* data) {
  if (list == NULL || data == NULL)
    return 0;

  dllist_node_t* node = gen_dllist_node(data);

  if (dllist_first_node(list, node))
    return 1;

  list->last->next = node;
  node->prev = list->last;
  list->last = node;

  list->size++;

  return 1;
}
short dllist_push_front(dllist_t* list, void* data) {
  if (list == NULL || data == NULL)
    return 0;

  dllist_node_t* node = gen_dllist_node(data);

  if (dllist_first_node(list, node))
    return 1;

  list->first->prev = node;
  node->next = list->first;
  list->first = node;

  list->size++;

  return 1;
}
// short dllist_pop_back(dllist_t* list, void** addr);
// short dllist_pop_front(dllist_t* list, void** addr);
// short dllist_insert(dllist_t* list, size_t index, void* data);
// short dllist_remove(dllist_t* list, size_t index, void** addr);
// void* dllist_get(dllist_t* list, size_t index);
// short dllist_free(dllist_t* list);

short dllist_first_node(dllist_t* list, dllist_node_t* node) {
  if (list->first != NULL || list->last != NULL)
    return 0;

  list->first = node;
  list->last = node;
  return 1;
}