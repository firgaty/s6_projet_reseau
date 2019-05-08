#include "test.h"
#include <arpa/inet.h>

int test_msg() {
  tlv_t* t = new_tlv_padn(10);
  print_tlv(t);

  // printf("%d: %c ; %d\n", TLV_PADN, (unsigned char)TLV_PADN,
  //        (int)((unsigned char)TLV_PADN));

  tlv_t** ts = malloc(sizeof(tlv_t*) * 2);
  ts[0] = t;
  ts[1] = new_tlv_hello(true, 1234567890, 982123);
  msg_t* m = new_msg(ts, 2);

  // // printf("Size of: %d\n", sizeof(struct pad_n));
  // // printf("Size of: %d\n", sizeof(struct hello));

  print_msg(m);

  // char** addr = malloc(sizeof(char*));
  sbuff_t* sb = new_sbuff();

  printf("Conversion to char[]...\n");
  // msg_to_char_array(m, addr);
  serial_msg(m, sb);
  sbuff_t* db = new_dsbuff(sb->data, sb->next);

  printf("Conversion to msg_t...\n");
  // sbuff_reserve_space(db, sb->size);
  // printf("%.*s\n", db->size, db->data);
  // printf("Size: %ld | Next: %ld\n", db->size, db->next);
  // sb->next = 0;

  msg_t* m2 = dserial_msg(db);
  // printf("Size: %ld | Next: %ld\n", db->size, db->next);

  // msg_t** m_addr = malloc(sizeof(msg_t*));

  // char_array_to_msg(*addr, m_addr);

  // // bug ici...
  printf("bug\n");
  print_msg(m2);

  free_msg(m);
  free_msg(m2);
  free_sbuff(sb);
  free(db);
  return 1;
}

int test_dllist() {
  printf("###DLLIST TEST###\n");

  dllist_t* l = new_dllist(DLL_INT);

  int a[5] = {1, 2, 3, 4, 5};

  dllist_push_back(l, DLL_INT, &a[0]);
  dllist_push_back(l, DLL_INT, &a[1]);
  dllist_push_front(l, DLL_INT, &a[2]);
  dllist_insert(l, 2, DLL_INT, &a[3]);

  dllist_node_t* n = l->first;

  printf("%ld\n", l->size);

  for (int i = 0; i < l->size; i++, n = n->next) {
    printf("%d | ", *((int*)n->data));
  }

  int* b = (int*)dllist_remove(l, 2);

  printf("\n%d\n", *b);

  n = l->first;
  for (int i = 0; i < l->size; i++, n = n->next) {
    printf("%d | ", *((int*)n->data));
  }

  printf("\n%d\n", *(int*)dllist_get(l, 2));

  printf("\n%d\n", *(int*)dllist_pop_back(l));

  return 1;
}

int test_neighbour_map() {
  neighbour_map_t* m = new_neighbour_map();

  neighbour_entry_t* e1 = new_neighbour_entry("ffffffffffffffff", 100);
  neighbour_entry_t* e2 = new_neighbour_entry("ffffffffffffeeee", 100);
  neighbour_entry_t* e3 = new_neighbour_entry("ffffffffeeeeeeee", 100);
  neighbour_entry_t* e4 = new_neighbour_entry("ffffeeeeeeeeeeee", 100);
  neighbour_entry_t* e5 = new_neighbour_entry("eeeeeeeeeeeeeeee", 100);

  map_add_neighbour_entry(m, e1);
  map_add_neighbour_entry(m, e2);
  map_add_neighbour_entry(m, e3);
  map_add_neighbour_entry(m, e4);
  map_add_neighbour_entry(m, e5);

  print_neighbour_map(m);

  return 1;
}

int test_connect(char* port) {
  // struct in6_addr addr;
  // char addr_[16];

  // // char* ip = malloc(sizeof(char) * 16);

  // int rc = inet_pton(AF_INET6, "2001:660:3301:9200::51c2:1b9b", &addr_);
  // if (rc <= 0)
  //   printf("erreur conversion ip\n");
  // // printf("%.16s", addr.__in6_u.__u6_addr8);
  // char ip_str[INET6_ADDRSTRLEN];
  // inet_ntop(AF_INET6, addr_, ip_str, INET6_ADDRSTRLEN);
  // printf("%s\n", ip_str);
  // struct sockaddr_in6 a;
  // a.sin6_port;

  // uint16_t port = 1212;
  // tlv_t* t = new_tlv_hello(false, 1234567890, NULL);
  // msg_t* m = new_msg(&t, 1);

  // //print_msg(m);

  // // char** addr = malloc(sizeof(char*));
  // sbuff_t* sb = new_sbuff();

  // printf("Conversion to char[]...\n");
  // // msg_to_char_array(m, addr);
  // serial_msg(m, sb);

  // udp_send(addr_, port, sb);

  // add_nbr("jch.irif.fr", "1212");
  int i = 10;
  wait(&i);
  add_nbr((unsigned char*)"jch.irif.fr", port);
  return 0;
}