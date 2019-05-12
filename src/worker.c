#include <time.h>

#include "worker.h"

void* worker_loop() {
  while (1) {
    printf("worker\n");
    printf("cur\n");
    neighbour_map_t *cur = get_cur_neighbours();
    printf("pot\n");
    neighbour_map_t *pot = get_pot_neighbours();
    printf("list\n");
    dllist_t *msg_list = get_msg_list();
    printf("map\n");
    data_map_t *msg_map = get_msg_map();

    worker_iter_nbr(cur, pot, msg_list, msg_map);

    pthread_mutex_unlock(&cur_neighbours_lock);
    pthread_mutex_unlock(&pot_neighbours_lock);
    pthread_mutex_unlock(&msg_list_lock);
    pthread_mutex_unlock(&msg_map_lock);
    nanosleep((const struct timespec[]){{0, 500000000L}}, NULL);
  }

  printf("exit loop\n");
}

void worker_iter_nbr(neighbour_map_t* cur,
                     neighbour_map_t* pot,
                     dllist_t* msg_list,
                     data_map_t* msg_map) {
  const char* key;
  map_iter_t iter = map_iter(&m);
  int rc;

  while ((key = map_next(cur, &iter))) {
    neighbour_entry_t** e = map_get(cur, key);
    if(e == NULL)
      return;
    rc = worker_iter_msg(cur, pot, msg_list, msg_map, *e);
    if (rc < 0) {
      dllist_empty(e[0]->msg_to_send, false);
    }
    printf("Neighbour moved to potential.");
    send_go_away((struct sockaddr_in6 *)e[0]->addr->ai_addr);
    map_transfer_neighbour(cur, pot, (char *)key);
  }

  return;
}

int worker_iter_msg(neighbour_map_t* cur,
                    neighbour_map_t* pot,
                    dllist_t* msg_list,
                    data_map_t* msg_map,
                    neighbour_entry_t* nbr) {
  dllist_t* list = nbr->msg_to_send;

  if (list->first == NULL)
    return 0;

  tlv_t* tab[64];
  size_t count = 0;
  size_t size = 0;
  dllist_node_t* n = list->first;
  time_t mytime = time(NULL);
  int status = 1;

  while ((n)) {
    dllist_msg_t* m = (dllist_msg_t*)n->data;
    if (m->time_send < mytime) {
      // envoit d'un message si l taille atteind le PMTU.
      if (size + m->body->data_len + 4 > nbr->pmtu) {
        msg_t* msg = new_msg(tab, count);
        send_msg((struct sockaddr_in6*)nbr->addr->ai_addr, msg);
        free_msg(msg, false);
        for (int i = 0; i < count; i++) {
          free(tab[i]);
          tab[i] = NULL;
        }
        count = 0;
        size = 0;
      }

      if (m->tries >= 5) {
        status = -1;
        break;
      }

      tab[count] = new_tlv_data_b(m->body);
      count++;
      size += tab[count]->length + 2;
      // incrementation du nombre de tentatives.
      m->tries++;
      set_time_send(m->tries);
    }
  }

  msg_t* msg = new_msg(tab, count);
  send_msg((struct sockaddr_in6*)nbr->addr->ai_addr, msg);
  free_msg(msg, false);
  for (int i = 0; i < count; i++) {
    free(tab[i]);
    tab[i] = NULL;
  }

  return status;
}
