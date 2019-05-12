#include "shared_resources.h"

neighbour_map_t* g_cur_neighbours = NULL;
neighbour_map_t* g_pot_neighbours = NULL;
data_map_t* g_msg_map = NULL;
dllist_t* g_msg_list = NULL;

neighbour_map_t* get_cur_neighbours() {
  pthread_mutex_lock(&cur_neighbours_lock);

  if (g_cur_neighbours == NULL) {
    g_cur_neighbours = new_neighbour_map();
  }
  return g_cur_neighbours;
}

neighbour_map_t* get_pot_neighbours() {
  pthread_mutex_lock(&pot_neighbours_lock);

  // static neighbour_map_t* m = NULL;
  if (g_pot_neighbours == NULL) {
    g_pot_neighbours = new_neighbour_map();
  }
  return g_pot_neighbours;
}

data_map_t* get_msg_map() {
  pthread_mutex_lock(&msg_map_lock);

  // static data_map_t* m = NULL;
  if (g_msg_map == NULL) {
    g_msg_map = new_data_map();
  }
  return g_msg_map;
}

dllist_t* get_msg_list() {
  pthread_mutex_lock(&msg_list_lock);

  // static dllist_t* l = NULL;
  if (g_msg_list == NULL) {
    g_msg_list = new_dllist(DLL_STRING);
  }
  return g_msg_list;
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
