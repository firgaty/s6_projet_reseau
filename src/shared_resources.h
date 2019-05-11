/**
 * @file shared_resources.h
 * @author Félix Desmaretz 
 * @brief All shared resources, i.e. all variables that are
 * accessed by multiple files.
 * @version 0.1
 * @date 2019-05-04
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef SHARED_RESOURCES_H_
#define SHARED_RESOURCES_H_

#include <pthread.h>

#include "types.h"

pthread_mutex_t cur_neighbours_lock;
pthread_mutex_t pot_neighbours_lock;
pthread_mutex_t msg_list_lock;

/**
 * @brief Get the cur neighbours object
 * 
 * Il ne faut pas oublier de déverouiller 
 * le mutex cur_neighbours_lock.
 * 
 * @return neighbour_map_t* 
 */
neighbour_map_t* get_cur_neighbours();

/**
 * @brief Get the pot neighbours object
 * 
 * Il ne faut pas oublier de déverouiller 
 * le mutex pot_neighbours_lock.
 * 
 * @return neighbour_map_t* 
 */
neighbour_map_t* get_pot_neighbours();

/**
 * @brief Get the msg list object
 * 
 * Il ne faut pas oublier de déverouiller 
 * le mutex msg_list_lock.
 * 
 * @return dllist_t* 
 */
dllist_t* get_msg_list();

// ID of the client.
uint64_t client_id;
void new_client_id();
uint64_t get_client_id();

char client_name[1024];
void new_client_name(char* name, size_t len);
char* get_client_name();

#endif  // !SHARED_RESOURCES_H_
