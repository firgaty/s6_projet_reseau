#include "neighbour_map.h"

// short map_add_new_neighbour(neighbour_map_t* map, char* ip, uint16_t port) {
//   char* key = new_neighbour_key(ip, port);
//   neighbour_entry_t** entry = malloc(sizeof(neighbour_entry_t*));
//   entry = map_get(map, key);
//   short out = 0;

//   if (!(entry)) {
//     map_set(map, key, new_neighbour_entry);
//     out = 1;
//   }

//   free(key);
//   return out;
// }

short map_add_neighbour_entry(neighbour_map_t* map, neighbour_entry_t* e) {
  struct sockaddr_in6* addr = (struct sockaddr_in6*)e->addr->ai_addr;
  char* str_ip[INET6_ADDRSTRLEN];
  inet_ntop(AF_INET6, &addr->sin6_addr, str_ip, INET6_ADDRSTRLEN);
  char* key = new_neighbour_key(str_ip, htons(addr->sin6_port));
  neighbour_entry_t** entry = malloc(sizeof(neighbour_entry_t*));
  entry = map_get(map, key);

  short out = 0;

  if (!(entry)) {
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
  char* key = new_neighbour_key(ip, port);
  neighbour_entry_t* e = *map_get(map, key);
  if ((e))
    map_remove(map, key);
  free(key);
  return e;
}

void map_delete_entry(neighbour_map_t* map, char* ip, uint16_t port) {
  char* key = new_neighbour_key(ip, port);
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

char* new_neighbour_key(char* ip, uint16_t port) {
  size_t len = INET6_ADDRSTRLEN + sizeof(port) + 1;
  char* key = malloc(sizeof(char) * len);
  snprintf(key, len, "%s%u", ip, port);
  return key;  // Il faut free à chaque usage.
}

void print_neighbour_entry(neighbour_entry_t* e) {
  struct sockaddr_in6* addr = (struct sockaddr_in6*)e->addr->ai_addr;
  char* ip_str[INET6_ADDRSTRLEN];
  inet_ntop(AF_INET6, &addr->sin6_addr, ip_str, INET6_ADDRSTRLEN);
  printf("----\n");
  printf("IP: %s\n", 16, ip_str);
  printf("Port: %d\n", htons(addr->sin6_port));
  printf("Last HELLO short: %ld\n", e->last_short_hello);
  printf("Last HELLO long: %ld\n", e->last_long_hello);
}

void print_neighbour_map(neighbour_map_t* m) {
  printf("map:\n");
  char* key;
  map_iter_t iter = map_iter(m);

  while ((key = map_next(m, &iter))) {
    neighbour_entry_t** e = map_get(m, key);
    if ((*e))
      print_neighbour_entry(*e);
    else
      printf("No entry...\n");
  }
}

void map_send_hello(neighbour_map_t* m, time_t delay) {
  const char* key;
  map_iter_t iter = map_iter(m);

  while ((key = map_next(m, &iter))) {
  }
}

void map_update_hello(char* ip, uint16_t port) {}

void add_new_neighbour(unsigned char* host, char* port) {
  printf("add_nbr\n");
  struct addrinfo hint = {0};
  hint.ai_family = PF_INET6;
  hint.ai_flags = AI_V4MAPPED | AI_ALL;
  hint.ai_socktype = SOCK_DGRAM;

  struct addrinfo* ai;

  int rc = getaddrinfo((char*)host, port, &hint, &ai);
  if (rc != 0) {
    printf("%s\n", gai_strerror(rc));
    return;
  }

  struct addrinfo* p = ai;
  tlv_t* t = new_tlv_hello(false, client_id, 0);
  msg_t* m = new_msg(&t, 1);
  sbuff_t* sb = new_sbuff();
  serial_msg(m, sb);

  int status = 0;
  for (; p; p = p->ai_next) {
    // inet_ntop(AF_INET6, p->ai_addr, ip_str, p->ai_addrlen);
    // printf("%s\n", ip_str);
    status = udp_send((struct sockaddr_in6*)p->ai_addr, sb);
    if (status > 0) {
      printf("ajout voisins courants\n");
      neighbour_map_t *m = get_cur_neighbours();
      map_add_neighbour_entry(m, new_neighbour_entry(p));
      pthread_mutex_unlock(&cur_neighbours_lock);
    } else {
      printf("ajout voisind potentiels\n");
      neighbour_map_t *m = get_pot_neighbours();
      map_add_neighbour_entry(m, new_neighbour_entry(p));
      pthread_mutex_unlock(&pot_neighbours_lock);
    }
    status = 0;
  }
}