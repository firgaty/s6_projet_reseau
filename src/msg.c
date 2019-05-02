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

// les printf séparés sont fait exprès,
// ça permet de savoir exactement quels
// membres des structures ont des erreurs
// de lecture et/ou d'écriture sous valgrind.
void print_tlv(tlv_t* t) {
  printf("---\n");
  printf("Type: %d\n", (int)t->type);
  printf("Length: %d\n", t->length);

  switch ((int)t->type) {
    case TLV_PAD1:
      break;
    case TLV_PADN:
      print_padn(t->body);
      break;
    case TLV_HELLO:
      print_hello(t->body);
      break;
    case TLV_NEIGHBOUR:
      print_neighbour_body(t->body);
      break;
    case TLV_DATA:
      print_data(t->body);
      break;
    case TLV_ACK:
      print_ack(t->body);
      break;
    case TLV_GO_AWAY:
      print_go_away(t->body);
      break;
    case TLV_WARNING:
      print_warning(t->body);
    default:
      printf("TLV type not recognized...\n");
      break;
  }
  printf("---\n");
}

void print_padn(padn_body_t* b) {
  printf("TLV type: PadN\n");
  printf("N: %d\n", b->zeroes);
}

void print_hello(hello_body_t* b) {
  printf("Hello type: %d\n", (int)b->is_long);
  printf("Source ID: %ld\n", b->source_id);
  if (b->is_long)
    printf("Destination ID: %ld\n", b->dest_id);
}

void print_neighbour_body(neighbour_body_t* b) {
  printf("IP: %.*s\nPort: %u\n", 16, b->ip, b->port);
}

void print_data(data_body_t* b) {
  printf("Sender ID: %ld\nNonce: %d\nType: %d\nData:\n***\n%.*s\n***\n",
         b->sender_id, b->nonce, (int)b->type, (int)b->data_len, b->data);
}

void print_ack(ack_body_t* b) {
  printf("Sender ID: %ld\nNonce: %d\n", b->sender_id, b->nonce);
}

// TODO...
void print_go_away(go_away_body_t* b) {
  printf("Code: %d\nMessage:\n***\n%s\n***\n", (int)b->code, b->message);
}

void print_warning(warning_body_t* b) {
  printf("Message:\n***\n%s\n***\n", b->message);
}

void print_msg(msg_t* m) {
  printf("===\nMagic: %d\nVersion: %d\nLength: %d\nBody:\n", m->magic,
         m->version, m->length);
  for (int i = 0; i < m->tlv_nb; i++) {
    print_tlv(m->body[i]);
  }
  printf("===\n");
}