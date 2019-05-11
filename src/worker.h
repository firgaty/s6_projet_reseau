#ifndef WORKER_H_
#define WORKER_H_

#include "shared_resources.h"
#include "types.h"

void* worker_loop();

void worker_send_msg(neighbour_map_t* cur,
                     neighbour_map_t* pot,
                     dllist_t* lsg_list,
                     data_map_t* msg_map);

#endif  // !WORKER_H_