/**
 * @file list.h
 * @author Félix Desmaretz
 * @brief
 * @version 0.1
 * @date 2019-03-29
 *
 * @copyright Copyright (c) 2019
 *
 */
#ifndef LIST_H
#define LIST_H

#include <stdio.h>

/**
 * @brief Generic doubly linked list node.
 *
 */
typedef struct dllist_node_t {
  dllist_node_t* next;
  dllist_node_t* prev;
  void* data;
} dllist_node_t;

/**
 * @brief Generic doubly linked list.
 *
 */
typedef struct dllist_t {
  dllist_node_t* first;
  dllist_node_t* last;
  size_t size;
} dllist_t;

dllist_t* gen_dllist();
dllist_node_t* gen_dllist_node(void* data);
short dllist_push_back(dllist_t* list, void* data);
short dllist_push_front(dllist_t* list, void* data);
short dllist_pop_back(dllist_t* list, void** addr);
short dllist_pop_front(dllist_t* list, void** addr);
short dllist_insert(dllist_t* list, size_t index, void* data);
short dllist_remove(dllist_t* list, size_t index, void** addr);
void* dllist_get(dllist_t* list, size_t index);
short dllist_free(dllist_t* list);
short dllist_first_node(dllist_t *list, dllist_node_t *node);

#endif  // !LIST_H