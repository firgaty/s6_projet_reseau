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

void print_tlv(tlv_t* t) {
  printf("---\n");
  switch ((int)t->type) {
    case TLV_PAD1:
      printf("TLV type: Pad1\n");
      break;
    case TLV_PADN:
      printf("TLV type: PadN\nLength: %d\nN: %d\n", t->length,
             t->body.pad_n.zeroes);
      break;
    case TLV_HELLO:
      printf("TLV type: Hello\nLength: %d\nHello type: %d\nSource ID: %ld\n",
             t->length, (int)t->body.hello.type, t->body.hello.source_id);
      if (t->body.hello.type == LONG_HELLO)
        printf("Destination ID: %ld\n", t->body.hello.dest_id);
      break;
    case TLV_NEIGHBOUR:
      printf("TLV type: Neighbour\nLength: %d\nIP: %.*s\nPort: %u\n", t->length,
             16, t->body.neighbour.ip, t->body.neighbour.port);
      break;
    case TLV_DATA:
      printf(
          "TLV type: Data\nLength: %d\nSender ID: %ld\nNonce: %d\nType: "
          "%d\nData:\n***\n%s\n***\n",
          t->length, t->body.data.sender_id, t->body.data.nonce,
          (int)t->body.data.type, t->body.data.data);
      break;
    case TLV_ACK:
      printf("TLV type: Ack\nLength: %d\nSender ID: %ld\nNonce: %d\n",
             t->length, t->body.ack.sender_id, t->body.ack.nonce);
      break;
    case TLV_GO_AWAY:
      printf("TLV type: GoAway\nLength: %d\nCode: %d\nMessage:\n***\n%s\n***\n",
             t->length, (int)t->body.go_away.code, t->body.go_away.message);
      break;
    case TLV_WARNING:
      printf("TLV type: Warning\nLength: %d\nMessage:\n***\n%s\n***\n",
             t->length, t->body.warning.message);
    default:
      perror("TLV type not recognized...");
      break;
  }
  printf("---\n");
}

void print_msg(msg_t* m) {
  printf("===\nMagic: %d\nVersion: %d\nLength: %d\nBody:\n", m->magic,
         m->version, m->length);
  for (int i = 0; i < m->tlv_nb; i++) {
    print_tlv(m->body[i]);
  }
  printf("===\n");
}

// TODO
size_t msg_to_char_array(msg_t* m, char** addr) {
  unsigned long ptr = 0;
  *addr = malloc(sizeof(char) * (m->length + 2));

  (*addr)[ptr++] = m->magic;
  (*addr)[ptr++] = m->version;
  (*addr)[ptr++] = (m->length >> 8) & 0xFF;
  (*addr)[ptr++] = m->length & 0xFF;
  // TODO TLV
  for (int i = 0; i < m->tlv_nb; i++) {
    // m->length + 4 est dû au fait que le contenu du message commence au 5e
    // octet.
    tlv_to_char_array(m->body[i], addr, &ptr, m->length + 4);
    printf("added ! %d\n", ptr);
  }
  return 0;
}

// TODO ! SIZE il faut le calculer !
size_t char_array_to_msg(char* s, msg_t** addr) {
  size_t size = 0;
  unsigned long ptr = 0;
  *addr = malloc(sizeof(msg_t));
  (*addr)->magic = s[ptr++];
  (*addr)->version = s[ptr++];
  (*addr)->length = s[ptr++] << 8;
  (*addr)->length = s[ptr++] + (*addr)->length;
  // TODO TLV
  tlv_t** temp_ts = malloc(sizeof(tlv_t*));
  tlv_t** temp_ts2 = malloc(sizeof(tlv_t*) * MSG_TLV_NB_DEF);
  int nb = 0;
  // to remove.
  printf("length : %d\n", (*addr)->length);
  for (; ptr < (*addr)->length - 3; nb++) {
    printf("ptr: %d\n", ptr);
    char_array_to_tlv(s, temp_ts, &ptr, (*addr)->length + 4);
    temp_ts2[nb] = *temp_ts;
    print_tlv(*temp_ts);
  }

  (*addr)->body = malloc(sizeof(tlv_t*) * nb);
  for (int i = 0; i < nb; i++) {
    (*addr)->body[i] = temp_ts2[i];
  }

  (*addr)->tlv_nb = nb;

  return size;
}

size_t tlv_to_char_array(tlv_t* t,
                         char** addr,
                         unsigned long* ptr,
                         size_t max_ptr) {
  (*addr)[(*ptr)++] = t->type;
  (*addr)[(*ptr)++] = t->length;

  switch (t->type) {
    case TLV_PAD1:
      return 0;
      break;
    case TLV_PADN:
      memset((*addr + *ptr), 0, t->body.pad_n.zeroes);
      *ptr += t->body.pad_n.zeroes;
      break;
    case TLV_HELLO:
      memcpy((*addr + *ptr), &(t->body.hello.source_id), sizeof(uint64_t));
      *ptr += sizeof(uint64_t);
      if (t->body.hello.type) {
        memcpy((*addr + *ptr), &(t->body.hello.dest_id), sizeof(uint64_t));
        *ptr += sizeof(uint64_t);
      }
      break;
    case TLV_NEIGHBOUR:
      memcpy((*addr + *ptr), (t->body.neighbour.ip),
             sizeof(unsigned char) * 16);
      *ptr += sizeof(unsigned char) * 16;
      memcpy((*addr + *ptr), &(t->body.neighbour.port), sizeof(uint16_t));
      *ptr += sizeof(uint16_t);
      break;
    case TLV_DATA:
      // TODO
      memcpy((*addr + *ptr), &(t->body.data), t->length);
      *ptr += sizeof(struct data);
      break;
    case TLV_ACK:
      memcpy((*addr + *ptr), &(t->body.ack), t->length);
      *ptr += t->length;
      break;
    case TLV_GO_AWAY:
      // TODO
      memcpy((*addr + *ptr), &(t->body.go_away), t->length);
      break;
    case TLV_WARNING:
      // TODO
      memcpy((*addr + *ptr), &(t->body.warning), t->length);
      break;
    default:
      return 0;
      break;
  }

  // TODO
  return t->length;
}

size_t char_array_to_tlv(char* s,
                         tlv_t** addr,
                         unsigned long* ptr,
                         size_t max_ptr) {
  *addr = malloc(sizeof(tlv_t*));
  (*addr)->type = s[(*ptr)++];
  (*addr)->length = s[(*ptr)++];

  switch ((*addr)->type) {
    case TLV_PAD1:
      break;
    case TLV_PADN:
      (*addr)->body.pad_n.zeroes = (*addr)->length;
      *ptr += (*addr)->body.pad_n.zeroes;
      break;
    case TLV_HELLO:
      (*addr)->body.hello.type = (*addr)->length / sizeof(uint64_t) == 2;
      char_to_member(s, ptr, &((*addr)->body.hello.source_id),
                     sizeof(uint64_t));
      if ((*addr)->body.hello.type == LONG_HELLO)
        char_to_member(s, ptr, &((*addr)->body.hello.dest_id),
                       sizeof(uint64_t));
      break;
    case TLV_NEIGHBOUR:
      char_to_member(s, ptr, (*addr)->body.neighbour.ip,
                     sizeof(unsigned char) * 16);
      char_to_member(s, ptr, &((*addr)->body.neighbour.port), sizeof(uint16_t));
      break;
    case TLV_DATA:
      // TODO
      memcpy(&((*addr)->body.data), (s + *ptr), (*addr)->length);
      break;
    case TLV_ACK:
      memcpy(&((*addr)->body.data), (s + *ptr), (*addr)->length);
      *ptr += (*addr)->length;
      break;
    case TLV_GO_AWAY:
      // TODO
      memcpy(&((*addr)->body.go_away), (s + *ptr), (*addr)->length);
      break;
    case TLV_WARNING:
      // TODO
      memcpy(&((*addr)->body.warning), (s + *ptr), (*addr)->length);
      break;
    default:
      // TODO : implement error.
      break;
  }

  // TODO
  return 0;
}

size_t char_to_member(char* s, unsigned long* ptr, void* member, size_t size) {
  memcpy(member, (s + *ptr), size);
  (*ptr) += size;
  return size;
}

short* ack_from_data(tlv_t* data_tlv, tlv_t** ack_tlv) {
  if (data_tlv->type != TLV_DATA)
    return 0;
  *ack_tlv =
      gen_tlv_ack(data_tlv->body.data.sender_id, data_tlv->body.data.nonce);
  return 1;
}

uint32_t gen_nonce(uint64_t id) {
  time_t s;
  time(&s);

  return (s ^ 2) / ((uint32_t)id);
}