#ifndef WORKER_H_
#define WORKER_H_

#include "connect.h"
#include "shared_resources.h"
#include "types.h"

void* worker_loop();

void worker_iter_nbr(neighbour_map_t* cur,
                     neighbour_map_t* pot,
                     dllist_t* msg_list,
                     data_map_t* msg_map);

int worker_iter_msg(neighbour_map_t* cur,
                     neighbour_map_t* pot,
                     dllist_t* msg_list,
                     data_map_t* msg_map,
                     neighbour_entry_t* nbr);

#endif  // !WORKER_H_