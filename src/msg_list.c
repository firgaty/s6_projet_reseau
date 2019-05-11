#include "msg_list.h"
#include "shared_resources.h"

bool is_in_msg_list(data_body_t* b) {
  dllist_t* l = get_msg_list();
  if (l->first == NULL)
    return 0;
  dllist_node_t* n = l->first;
  char* key = new_data_key(b->sender_id, b->nonce);
  while ((n)) {
    if (strncmp((char*)n->data, key, 20) == 0) {
      pthread_mutex_unlock(&msg_list_lock);
      return true;
    }
    n = n->next;
  }
  pthread_mutex_unlock(&msg_list_lock);
  return false;
}

bool add_msg(data_body_t* b) {
  if (is_in_msg_list(b))
    return 0;

  print_message(b->data, b->data_len, MSG_USER);

  char* msg_key = new_data_key(b->sender_id, b->nonce);

  data_map_t* msg = get_msg_map();
  map_set(msg, msg_key, b);

  dllist_t* l = get_msg_list();
  dllist_push_front(l, DLL_STRING, msg_key);

  neighbour_map_t* m = get_cur_neighbours();
  const char* key;
  map_iter_t iter = map_iter(&m);

  while ((key = map_next(m, &iter))) {
    neighbour_entry_t* e = *map_get(m, key);
    dllist_msg_t* d = new_dllist_msg(msg_key, b);
    dllist_push_front(e->msg_to_send, DLL_INT, d);
  }
  pthread_mutex_unlock(&cur_neighbours_lock);
  pthread_mutex_unlock(&msg_map_lock);
  pthread_mutex_unlock(&msg_list_lock);
}

void rm_n_msg(size_t n) {
  for (int i = 0; i < n; i++) {
    if (!rm_msg())
      return;
  }
}

bool rm_msg() {
  // dllist_t *l = get_msg_list();
  // char *key = (char*)l->last->data;

  // neighbour_map_t *m = get_cur_neighbours();
  // const char *nkey;
  // map_iter_t iter = map_iter(&m);

  // while ((nkey = map_next(m, &iter))) {
  //   neighbour_entry_t *e = *map_get(m, key);
  //   dllist_msg_t *d = new_dllist_msg(nkey, b);
  //   dllist_push_front(e->msg_to_send, DLL_INT, d);
  // }
  return true;
}
