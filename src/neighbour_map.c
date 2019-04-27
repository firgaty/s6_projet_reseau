#include "neighbour_map.h"

short map_add_new_neighbour(neighbour_map_t* map, char* ip, uint16_t port) {
  char* key = gen_neighbour_key(ip, port);
  neighbour_entry_t **entry = malloc(sizeof(neighbour_entry_t*));
  entry = map_get(map, key);
  short out = 0;

  if (!(entry)) {
    map_set(map, key, gen_neighbour_entry(ip, port));
    out = 1;
  }

  free(key);
  return out;
}

short map_add_neighbour_entry(neighbour_map_t* map, neighbour_entry_t* e) {
  char* key = gen_neighbour_key(e->ip, e->port);
  neighbour_entry_t **entry = malloc(sizeof(neighbour_entry_t *));
  entry = map_get(map, key);
  
  short out = 0;
  
  if (!(entry))
  {
    map_set(map, key, e);
    out = 1;
  }

  free(entry);
  free(key);

  return out;
}

neighbour_entry_t* map_pop_neighbour(neighbour_map_t* map,
                                     char* ip,
                                     uint16_t port) {
  char* key = gen_neighbour_key(ip, port);
  neighbour_entry_t* e = *map_get(map, key);
  if ((e))
    map_remove(map, key);
  free(key);
  return e;
}

void map_delete_entry(neighbour_map_t* map, char* ip, uint16_t port) {
  char* key = gen_neighbour_key(ip, port);
  neighbour_entry_t* e = *map_get(map, key);
  if ((e)) {
    map_remove(map, key);
    free_neighbour_entry(e);
  }
}

short map_transfer_neighbour(neighbour_map_t* in,
                             neighbour_map_t* out,
                             char* ip,
                             uint16_t port) {
  neighbour_entry_t* e = map_pop_neighbour(in, ip, port);
  if (!(e))
    return 0;
  return map_add_neighbour_entry(out, e);
}

char* gen_neighbour_key(char* ip, uint16_t port) {
  // char* key = malloc(sizeof(char) * (16 + sizeof(port)));
  // memcpy(key, ip, 16);
  // memcpy(key + 16, port, sizeof(port));
  // return key;  // Il faut free à chaque usage.
  char* key = malloc(sizeof(char) * 18);
  snprintf(key, 18, "%.*s%u", 16, ip, port);
  return key;
}

void print_neighbour_entry(neighbour_entry_t* e) {
  printf("----\n");
  printf("IP: %.*s\n", 16, e->ip);
  printf("Port: %d\n", e->port);
  printf("Last HELLO short: %ld\n", e->last_short_hello);
  printf("Last HELLO long: %ld\n", e->last_long_hello);
}

void print_neighbour_map(neighbour_map_t* m) {
  printf("map:\n");
  const char* key;
  map_iter_t iter = map_iter(m);

  while ((key = map_next(m, &iter))) {
    printf("%s:\n", key);
    neighbour_entry_t** e = map_get(m, key);
    if ((*e))
      print_neighbour_entry(*e);
    else
      printf("No entry...\n");
  }
}