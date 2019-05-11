#include <time.h>

#include "worker.h"

void* worker_loop() {
  while (1) {
    neighbour_map_t* cur = get_cur_neighbours();
    neighbour_map_t* pot = get_pot_neighbours();
    dllist_t* msg_list = get_msg_list();
    data_map_t* msg_map = get_msg_map();

    pthread_mutex_unlock(&cur_neighbours_lock);
    pthread_mutex_unlock(&pot_neighbours_lock);
    pthread_mutex_unlock(&msg_list_lock);
    pthread_mutex_unlock(&msg_map_lock);
    nanosleep((const struct timespec[]){{0, 500000000L}}, NULL);
  }
}

void worker_send_msg(neighbour_map_t* cur,
                     neighbour_map_t* pot,
                     dllist_t* lsg_list,
                     data_map_t* msg_map) {
  return;
}
