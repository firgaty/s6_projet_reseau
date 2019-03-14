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
  out->length = n;
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

msg_t *gen_msg(tlv_t **ts, size_t ts_size) {
  msg_t *m = malloc(sizeof(msg_t));
  m->magic = MSG_MAGIC;
  m->version = MSG_VERSION;
  m->tlv_nb = ts_size;
  // TODO: Gérer la taille maximale d'un message.
  m->body = ts;
  m->length = 0;
  for (int i = 0; i < ts_size; i++) {
    m->length += 16 + ts[i]->length;
  }
  return m;
}

void print_tlv(tlv_t *t) {
  printf("---\n");
  switch ((int)t->type) {
  case PAD1:
    printf("TLV type: Pad1\n");
    break;
  case PADN:
    printf("TLV type: PadN\nLength: %d\nN: %d\n", t->length,
           t->body.pad_n.zeroes);
    break;
  case HELLO:
    printf("TLV type: Hello\nLength: %d\nHello type: %d\nSource ID: %ld\n",
           t->length, (int)t->body.hello.type, t->body.hello.source_id);
    if (t->body.hello.type == LONG_HELLO)
      printf("Destination ID: %ld\n", t->body.hello.dest_id);
    break;
  case NEIGHBOUR:
    printf("TLV type: Neighbour\nLength: %d\nIP: %s\nPort: %d\n", t->length,
           t->body.neighbour.ip, t->body.neighbour.port);
    break;
  case DATA:
    printf("TLV type: Data\nLength: %d\nSender ID: %ld\nNonce: %d\nType: "
           "%d\nData:\n***\n%s\n***\n",
           t->length, t->body.data.sender_id, t->body.data.nonce,
           (int)t->body.data.type, t->body.data.data);
    break;
  case ACK:
    printf("TLV type: Ack\nLength: %d\nSender ID: %ld\nNonce: %d\n", t->length,
           t->body.ack.sender_id, t->body.ack.nonce);
    break;
  case GO_AWAY:
    printf("TLV type: GoAway\nLength: %d\nCode: %d\nMessage:\n***\n%s\n***\n",
           t->length, (int)t->body.go_away.code, t->body.go_away.message);
    break;
  case WARNING:
    printf("TLV type: Warning\nLength: %d\nMessage:\n***\n%s\n***\n", t->length,
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

// TODO
size_t msg_to_char_array(msg_t *m, char **addr) {
  unsigned long ptr = 0;
  *addr = malloc(sizeof(char) * (m->length + 2));

  (*addr)[ptr++] = m->magic;
  (*addr)[ptr++] = m->version;
  (*addr)[ptr++] = (m->length >> 8) & 0xFF;
  (*addr)[ptr++] = m->length & 0xFF;
  // TODO TLV
  for (int i = 0; i < m->tlv_nb; i++, ptr++) {
    tlv_to_char_array(m->body[i], addr, &ptr, m->length - 3);
  }
  return 0;
}

// TODO
size_t char_array_to_msg(char *s, msg_t **addr) {
  size_t size = 0;
  unsigned long ptr = 0;
  *addr = malloc(sizeof(msg_t));
  (*addr)->magic = s[ptr++];
  (*addr)->version = s[ptr++];
  (*addr)->length = s[ptr++] << 8;
  (*addr)->length = s[ptr++] + (*addr)->length;
  // TODO TLV
  tlv_t **temp_ts = malloc(sizeof(tlv_t *));
  tlv_t **temp_ts2 = malloc(sizeof(tlv_t *) * MSG_TLV_NB_DEF);
  int nb = 0;
  for (int i = 0; i < (*addr)->length - 3; i++, ptr++, nb++) {
    char_array_to_tlv(s, temp_ts, &ptr, (*addr)->length - 3);
    temp_ts2[nb] = *temp_ts;
  }

  (*addr)->body = malloc(sizeof(tlv_t *) * nb);
  for (int i = 0; i < nb; i++) {
    (*addr)->body[i] = temp_ts2[i];
  }
  return size;
}

size_t tlv_to_char_array(tlv_t *t, char **addr, unsigned long *ptr,
                         size_t max_ptr) {
  (*addr)[(*ptr)++] = t->type;
  (*addr)[(*ptr)++] = t->length;

  switch (t->type) {
  case PAD1:
    return 0;
    break;
  case PADN:
    memcpy((*addr + *ptr), &(t->body.pad_n), t->length);
    break;
  case HELLO:
    memcpy((*addr + *ptr), &(t->body.hello), t->length);
    break;
  case NEIGHBOUR:
    memcpy((*addr + *ptr), &(t->body.neighbour), t->length);
    break;
  case DATA:
    memcpy((*addr + *ptr), &(t->body.data), t->length);
    break;
  case ACK:
    memcpy((*addr + *ptr), &(t->body.ack), t->length);
    break;
  case GO_AWAY:
    memcpy((*addr + *ptr), &(t->body.go_away), t->length);
    break;
  case WARNING:
    memcpy((*addr + *ptr), &(t->body.warning), t->length);
    break;
  default:
    return 0;
    break;
  }

  return t->length;
}

size_t char_array_to_tlv(char *s, tlv_t **addr, unsigned long *ptr,
                         size_t max_ptr) {
  *addr = malloc(sizeof(tlv_t *));
  (*addr)->type = s[(*ptr)++];
  (*addr)->length = s[(*ptr)++];

  switch ((*addr)->type) {
  case PAD1:
    break;
  case PADN:
    memcpy(&((*addr)->body.pad_n), (s + *ptr), (*addr)->length);
    break;
  case HELLO:
    memcpy(&((*addr)->body.hello), (s + *ptr), (*addr)->length);
    break;
  case NEIGHBOUR:
    memcpy(&((*addr)->body.neighbour), (s + *ptr), (*addr)->length);
    break;
  case DATA:
    memcpy(&((*addr)->body.data), (s + *ptr), (*addr)->length);
    break;
  case ACK:
    memcpy(&((*addr)->body.data), (s + *ptr), (*addr)->length);
    break;
  case GO_AWAY:
    memcpy(&((*addr)->body.go_away), (s + *ptr), (*addr)->length);
    break;
  case WARNING:
    memcpy(&((*addr)->body.warning), (s + *ptr), (*addr)->length);
    break;
  default:
    // TODO : implement error.
    break;
  }
  *ptr += (*addr)->length;
  return 0;
}
