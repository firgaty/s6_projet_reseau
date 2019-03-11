/**
 * @file msg.c
 * @author Félix Desmaretz
 * @brief
 * @version 0.1
 * @date 2019-03-11
 *
 * @copyright Copyright (c) 2019
 *
 */

#include <stdlib.h>
#include <time.h>

#include "msg.h"

tlv_t *gen_tlv_pad1() {
  tlv_t *out = malloc(sizeof(tlv_t));
  out->type = (unsigned char)PAD1;
  out->length = 0;
  return out;
}

tlv_t *gen_tlv_padn(unsigned int n) {
  tlv_t *out = malloc(sizeof(tlv_t));
  out->length = (unsigned char)PADN;
  out->length = n * 8;
  return out;
}

tlv_t *gen_tlv_hello(HELLO_TYPE type, unsigned long long source_id,
                     unsigned long long dest_id) {
  tlv_t *out = malloc(sizeof(tlv_t));
  out->type = (unsigned char)HELLO;
  out->length = 64 + type == LONG_HELLO ? 64 : 0;
  out->body.hello.source_id = source_id;
  out->body.hello.dest_id = type == LONG_HELLO ? dest_id : NULL;
  out->body.hello.type = (unsigned char)type;
  return out;
}

tlv_t *gen_tlv_neighbour(char *ip, unsigned int port) {
  tlv_t *out = malloc(sizeof(tlv_t));
  out->type = (unsigned char)NEIGHBOUR;
  out->length = 144;
  out->body.neighbour.ip = ip;
  out->body.neighbour.port = port;
  return out;
}

tlv_t *gen_tlv_data(unsigned long long sender_id, DATA_TYPE type, char *data) {
  tlv_t *out = malloc(sizeof(tlv_t));
  out->type = (unsigned char)DATA;
  // random nonce.
  out->body.data.nonce = rand();
  out->body.data.nonce = (out->body.data.nonce << 32) | rand();
}
