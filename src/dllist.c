#include "dllist.h"

short dllist_push_back(dllist_t* list, DLL_NODE_TYPE type, void* data) {
  if (list == NULL || data == NULL || type != list->type)
    return 0;

  dllist_node_t* node = gen_dllist_node(type, data);

  if (dllist_first_node(list, node))
    return 1;

  list->last->next = node;
  node->prev = list->last;
  list->last = node;

  list->size++;

  return 1;
}
short dllist_push_front(dllist_t* list, DLL_NODE_TYPE type, void* data) {
  if (list == NULL || data == NULL || type != list->type)
    return 0;

  dllist_node_t* node = gen_dllist_node(type, data);

  if (dllist_first_node(list, node))
    return 1;

  list->first->prev = node;
  node->next = list->first;
  list->first = node;

  list->size++;

  return 1;
}

short dllist_pop_back(dllist_t* list, void** addr) {
  if (dllist_is_empty(list))
    return 0;

  dllist_node_t* n = list->last;
  if (addr != NULL)
    *addr = list->last->data;

  if (dllist_last_node(list)) {
    list->first = NULL;
    list->last = NULL;
  } else {
    list->last = list->last->prev;
    list->last->next = NULL;
  }

  dllist_free_node(n, FALSE);

  list->size--;

  return 1;
}

short dllist_pop_front(dllist_t* list, void** addr) {
  if (dllist_is_empty(list))
    return 0;

  dllist_node_t* n = list->first;
  if (addr != NULL)
    *addr = list->first->data;

  if (dllist_last_node(list)) {
    list->first = NULL;
    list->last = NULL;
  } else {
    list->first = list->first->next;
    list->first->prev = NULL;
  }

  dllist_free_node(n, FALSE);

  list->size--;

  return 1;
}

short dllist_insert(dllist_t* list,
                    size_t index,
                    DLL_NODE_TYPE type,
                    void* data) {
  if (list == NULL || data == NULL || type != list->type)
    return 0;

  dllist_node_t *n, *node;

  if (index > 0) {
    if (index >= list->size)
      return dllist_push_back(list, type, data);

    n = gen_dllist_node(type, data);
    node = list->first;
    for (int i = 0; i <= index; i++, node = node->next) {
    }
    node->prev->next = n;
    n->prev = node->prev;
    node->prev = n;
    n->next = node;
  } else if (index < 0) {
    if (-index >= list->size)
      return dllist_push_front(list, type, data);

    n = gen_dllist_node(type, data);
    node = list->last;
    for (int i = 0; i <= -index; i++, node = node->prev) {
    }
    node->next->prev = n;
    n->next = node->next;
    node->next = n;
    n->prev = node;
  } else {
    dllist_push_front(list, type, data);
  }
}

short dllist_remove(dllist_t* list, size_t index, void** addr) {
  if (dllist_is_empty(list))
    return 0;
  if (index > 0 && index >= list->size || index < 0 && -index >= list->size)
    return 0;

  if (index + 1 == list->size)
    return dllist_pop_back(list, addr);
  if (index == 0 || -(index - 1) == list->size)
    return dllist_pop_front(list, addr);

  dllist_node_t* n;
  if (index > 0) {
    n = list->first;
    for (int i = 0; i <= index; i++, n = n->next) {
    }
  } else {
    n = list->last;
    for (int i = 0; i <= -index; i++, n = n->prev) {
    }
  }

  n->next->prev = n->prev;
  n->prev->next = n->next;

  if (addr != NULL)
    *addr = n->data;

  dllist_free_node(n, FALSE);

  list->size--;

  return TRUE;
}

void* dllist_get(dllist_t* list, size_t index) {
  if (dllist_is_empty(list))
    return NULL;
  if (index > 0 && index >= list->size || index < 0 && -index >= list->size)
    return NULL;

  if (index + 1 == list->size)
    return list->last->data;
  if (index == 0 || -(index - 1) == list->size)
    return list->last->data;

  dllist_node_t* n;
  if (index > 0) {
    n = list->first;
    for (int i = 0; i <= index; i++, n = n->next) {
    }
  } else {
    n = list->last;
    for (int i = 0; i <= -index; i++, n = n->prev) {
    }
  }

  return n->data;
}

short dllist_first_node(dllist_t* list, dllist_node_t* node) {
  if (list->first != NULL || list->last != NULL)
    return 0;

  list->first = node;
  list->last = node;
  return 1;
}

short dllist_last_node(dllist_t* list) {
  return list->first == list->last;
}

short dllist_is_empty(dllist_t* list) {
  return list->first != NULL || list->last != NULL;
}