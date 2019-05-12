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

  print_message(b->data, b->data_len);

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
  
  return 1;
}

void rm_n_msg(size_t n) {
  for (int i = 0; i < n; i++) {
    if (!rm_msg())
      return;
  }
}

// TODO test.
bool rm_msg() {
  dllist_t* l = get_msg_list();
  data_map_t* data_map = get_msg_map();
  char* key = (char*)dllist_pop_back(l);
  if (key == NULL) {
    printf("no message to delete...");
    pthread_mutex_unlock(&msg_list_lock);
    pthread_mutex_unlock(&msg_list_lock);
    return false;
  }
  data_body_t* body = *map_get(data_map, key);

  neighbour_map_t* m = get_cur_neighbours();
  const char* nkey;
  map_iter_t iter = map_iter(&m);

  while ((nkey = map_next(m, &iter))) {
    neighbour_entry_t* e = *map_get(m, nkey);
    rm_nbr_msg(body->sender_id, body->nonce, e->msg_to_send);
  }

  pthread_mutex_unlock(&msg_list_lock);
  pthread_mutex_unlock(&cur_neighbours_lock);
  pthread_mutex_unlock(&msg_map_lock);

  return true;
}

bool rm_nbr_msg(uint64_t id, uint32_t nonce, dllist_t* list) {
  if (list->first == NULL)
    return false;

  dllist_node_t* n = list->first;
  bool found = false;
  int index = 0;
  for (; (n); n = n->next, index++) {
    dllist_msg_t* d = (dllist_msg_t*)n->data;
    if (d->body->sender_id == id && d->body->nonce == nonce) {
      found = true;
      break;
    }
  }

  if (found) {
    dllist_remove(list, index);
    return true;
  }

  return false;
}
