/**
 * @file neighbour_map.h
 * @author Félix Desmaretz
 * @brief
 * @version 0.1
 * @date 2019-04-26
 *
 * @copyright Copyright (c) 2019
 *
 */

#ifndef NEIGHBOUR_MAP_H_
#define NEIGHBOUR_MAP_H_

#include <pthread.h>

#include "map.h"
#include "shared_resources.h"
#include "types.h"
#include "connect.h"

/**
 * @brief Adds a new neighbour form ip and port.
 *
 * Only adds a neighbour if nonexistent in the map.
 *
 * @param map Map to use.
 * @param ip IP.
 * @param port Port.
 * @return short 1 if added else 0.
 */
short map_add_new_neighbour(neighbour_map_t* map, char* ip, uint16_t port);

/**
 * @brief Adds a neighbour entry.
 *
 * Only adds a neighbour if nonexistent in the map.
 *
 * @param map Map to use.
 * @param e Entry to add.
 * @return short 1 if added else 0.
 */
short map_add_neighbour_entry(neighbour_map_t* map, neighbour_entry_t* e);

/**
 * @brief Removes a neighbour from a map and returns it.
 *
 * To delete the entry directly see map_delete_entry()
 *
 * @param map Map to use.
 * @param ip IP.
 * @param port Port.
 * @return neighbour_entry_t* Entry removed.
 */
neighbour_entry_t* map_pop_neighbour(neighbour_map_t* map,
                                     char* ip,
                                     uint16_t port);

/**
 * @brief Deletes and frees allocated resources to an entry in a map.
 *
 * @param map Map to use.
 * @param ip IP.
 * @param port Port.
 */
void map_delete_entry(neighbour_map_t* map, char* ip, uint16_t port);

/**
 * @brief Transfers an entry from a list to another.
 *
 * @param in Map to take the entry from.
 * @param out Map to transfer the entry to.
 * @param ip IP.
 * @param port Port.
 * @return short 1 if success else 0.
 */
short map_transfer_neighbour(neighbour_map_t* in,
                             neighbour_map_t* out,
                             char* ip,
                             uint16_t port);

void print_neighbour_entry(neighbour_entry_t* e);
void print_neighbour_map(neighbour_map_t* m);

void map_send_hello(neighbour_map_t* m, time_t delay);
void map_update_hello(char* ip, uint16_t port);

void add_new_neighbour(unsigned char* host, char* port);

#endif  // NEIGHBOUR_MAP_H_
