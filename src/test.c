#include "test.h"

int test_msg() {
  tlv_t* t = gen_tlv_padn(10);
  print_tlv(t);

  printf("%d: %c ; %d\n", TLV_PADN, (unsigned char)TLV_PADN,
         (int)((unsigned char)TLV_PADN));

  tlv_t** ts = malloc(sizeof(tlv_t*) * 2);
  ts[0] = t;
  ts[1] = gen_tlv_hello(LONG_HELLO, 1234567890, 982123);
  msg_t* m = gen_msg(ts, 2);

  printf("Size of: %d\n", sizeof(struct pad_n));
  printf("Size of: %d\n", sizeof(struct hello));

  print_msg(m);

  char** addr = malloc(sizeof(char*));

  printf("Conversion to char[]...\n");
  msg_to_char_array(m, addr);

  printf("Conversion to msg_t...\n");
  msg_t** m_addr = malloc(sizeof(msg_t*));

  char_array_to_msg(*addr, m_addr);

  print_msg(*m_addr);

  unsigned long ptr = 0;
  tlv_to_char_array(t, addr, &ptr, 0);
  printf("%ld\n", ptr);
  ptr = 0;

  char_array_to_tlv(*addr, ts, &ptr, 0);
  print_tlv(*ts);
  return 0;
}

int test_dllist() {
  printf("###DLLIST TEST###\n");

  dllist_t* l = gen_dllist(DLL_INT);

  int a[5] = {1, 2, 3, 4, 5};

  dllist_push_back(l, DLL_INT, &a[0]);
  dllist_push_back(l, DLL_INT, &a[1]);
  dllist_push_front(l, DLL_INT, &a[2]);
  dllist_insert(l, 2, DLL_INT, &a[3]);

  dllist_node_t* n = l->first;

  printf("%d\n", l->size);

  for (int i = 0; i < l->size; i++, n = n->next) {
    printf("%d | ", *((int*)n->data));
  }

  int* b = (int*)dllist_remove(l, 2);

  printf("\n%d\n", *b);

  n = l->first;
  for (int i = 0; i < l->size; i++, n = n->next) {
    printf("%d | ", *((int*)n->data));
  }

  printf("\n%d\n", *(int *)dllist_get(l, 2));

  printf("\n%d\n", *(int *)dllist_pop_back(l));

  return 1;
}