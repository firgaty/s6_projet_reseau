#include "shared_resources.h"

neighbour_map_t* get_cur_neighbours() {
  pthread_mutex_lock(&cur_neighbours_lock);

  static neighbour_map_t* m = NULL;
  if (m == NULL) {
    m = new_neighbour_map();
  }
  return m;
}

neighbour_map_t* get_pot_neighbours() {
  pthread_mutex_lock(&pot_neighbours_lock);

  static neighbour_map_t* m = NULL;
  if (m == NULL) {
    m = new_neighbour_map();
  }
  return m;
}

dllist_t* get_msg_list() {
  pthread_mutex_lock(&msg_list_lock);

  static dllist_t* l = NULL;
  if (l == NULL) {
    l = new_dllist(DLL_MSG);
  }
  return l;
}

void new_client_id() {
  // On suppose que l'on est toujours sur un system x64.
  client_id = rand();
}

uint64_t get_client_id() {
  return client_id;
}

void new_client_name(char* name, size_t len) {
  snprintf(client_name, len, "%s", name);
}

char* get_client_name() {
  if (client_name == NULL) {
    return NULL;
  }
  return client_name;
}
