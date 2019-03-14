#include "test.h"

int test_msg() {
    tlv_t *t = gen_tlv_padn(10);
    print_tlv(t);

    printf("%d: %c ; %d\n", PADN, (unsigned char)PADN, (int)((unsigned char)PADN));

    tlv_t **ts = malloc(sizeof(tlv_t*));
    ts[0] = t;
    msg_t *m = gen_msg(ts, 1);

    print_msg(m);

    char **addr = malloc(sizeof(char*));

    msg_to_char_array(m, addr);

    printf("%s\n", *addr);

    msg_t **m_addr = malloc(sizeof(msg_t*));

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