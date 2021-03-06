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

#define FALSE (0)
#define TRUE (!FALSE)

#include "types.h"

short dllist_push_back(dllist_t* list, DLL_NODE_TYPE type, void* data);
short dllist_push_front(dllist_t* list, DLL_NODE_TYPE type, void* data);
void* dllist_pop_back(dllist_t* list);
void* dllist_pop_front(dllist_t* list);
short dllist_insert(dllist_t* list,
                    size_t index,
                    DLL_NODE_TYPE type,
                    void* data);
void* dllist_remove(dllist_t* list, size_t index);
void* dllist_get(dllist_t* list, size_t index);
short dllist_first_node(dllist_t* list, dllist_node_t* node);
short dllist_last_node(dllist_t* list);
void dllist_empty(dllist_t *list, bool erase_data);

#endif // !LIST_H