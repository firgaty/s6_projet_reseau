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
  out->type = (unsigned char)PADN;
  out->length = n * 8;
  out->body.pad_n.zeroes = n;
  return out;
}

tlv_t *gen_tlv_hello(HELLO_TYPE type, uint64_t source_id, uint64_t dest_id) {
  tlv_t *out = malloc(sizeof(tlv_t));
  out->type = (unsigned char)HELLO;
  out->length = 64 + type == LONG_HELLO ? 64 : 0;
  out->body.hello.source_id = source_id;
  out->body.hello.dest_id = type == LONG_HELLO ? dest_id : 0;
  out->body.hello.type = (unsigned char)type;
  return out;
}

tlv_t *gen_tlv_neighbour(char *ip, uint16_t port) {
  tlv_t *out = malloc(sizeof(tlv_t));
  out->type = (unsigned char)NEIGHBOUR;
  out->length = 144;
  out->body.neighbour.ip = ip;
  out->body.neighbour.port = port;
  return out;
}

// TODO: Implémenter length.
tlv_t *gen_tlv_data(uint64_t sender_id, uint32_t nonce, DATA_TYPE type,
                    char *data) {
  tlv_t *out = malloc(sizeof(tlv_t));
  out->type = (unsigned char)DATA;
  out->length = 8 * strlen(data); // TODO: Pas content de ceci.
  out->body.data.sender_id = sender_id;
  out->body.data.nonce = nonce;
  out->body.data.type = (unsigned char)type;
  out->body.data.data = data;
  return out;
}

tlv_t *gen_tlv_ack(uint64_t sender_id, uint32_t nonce) {
  tlv_t *out = malloc(sizeof(tlv_t));
  out->type = (unsigned char)ACK;
  out->length = 192;
  out->body.ack.nonce = nonce;
  out->body.ack.sender_id = sender_id;
  return out;
}

tlv_t *gen_tlv_go_away(char code, short contains_message, char *message,
                       size_t message_len) {
  tlv_t *out = malloc(sizeof(tlv_t));
  out->type = (unsigned char)GO_AWAY;
  out->length = 8;
  out->body.go_away.code = code;
  if (contains_message) {
    out->body.go_away.message = message;
    out->length += strlen(message) * 8;
  }
  return out;
}

tlv_t *gen_tlv_warning(char *message, size_t message_len) {
  tlv_t *out = malloc(sizeof(tlv_t));
  out->type = (unsigned char)WARNING;
  out->length = strlen(message) * 8;
  return out;
}

void print_tlv(tlv_t *t) {
  printf("---\n");
  switch ((int)t->type) {
  case PAD1:
    printf("TLV type: Pad1\n");
    break;
  case PADN:
    printf("TLV type: PadN\nN: %d\n", t->body.pad_n.zeroes);
    break;
  case HELLO:
    printf("TLV type: Hello\nHello type: %d\nSource ID: %ld\n",
           (int)t->body.hello.type, t->body.hello.source_id);
    if (t->body.hello.type == LONG_HELLO)
      printf("Destination ID: %ld\n", t->body.hello.dest_id);
    break;
  case NEIGHBOUR:
    printf("TLV type: Neighbour\nIP: %s\nPort: %d\n", t->body.neighbour.ip,
           t->body.neighbour.port);
    break;
  case DATA:
    printf("TLV type: Data\nSender ID: %ld\nNonce: %d\nType: "
           "%d\nData:\n***\n%s\n***\n",
           t->body.data.sender_id, t->body.data.nonce, (int)t->body.data.type,
           t->body.data.data);
    break;
  case ACK:
    printf("TLV type: Ack\nSender ID: %ld\nNonce: %d\n", t->body.ack.sender_id,
           t->body.ack.nonce);
    break;
  case GO_AWAY:
    printf("TLV type: GoAway\nCode: %d\nMessage:\n***\n%s\n***\n",
           (int)t->body.go_away.code, t->body.go_away.message);
    break;
  case WARNING:
    printf("TLV type: Warning\nMessage:\n***\n%s\n***\n",
           t->body.warning.message);
  default:
    perror("TLV type not recognized...");
    break;
  }
  printf("---\n");
}

void print_msg(msg_t *m) {
  printf("===\nMagic: %d\nVersion: %d\nLength: %d\nBody:\n", m->magic,
         m->version, m->length);
  for (int i = 0; i < m->tlv_nb; i++) {
    print_tlv(m->body[i]);
  }
  printf("===\n");
}
