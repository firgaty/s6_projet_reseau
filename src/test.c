#include "test.h"

int test_msg() {
  tlv_t* t = gen_tlv_padn(10);
  print_tlv(t);

  printf("%d: %c ; %d\n", PADN, (unsigned char)PADN,
         (int)((unsigned char)PADN));

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