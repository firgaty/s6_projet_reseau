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

// // TODO
// size_t msg_to_char_array(msg_t* m, char** addr) {
//   unsigned long ptr = 0;
//   *addr = malloc(sizeof(char) * (m->length + 2));

//   (*addr)[ptr++] = m->magic;
//   (*addr)[ptr++] = m->version;
//   (*addr)[ptr++] = (m->length >> 8) & 0xFF;
//   (*addr)[ptr++] = m->length & 0xFF;
//   // TODO TLV
//   for (int i = 0; i < m->tlv_nb; i++) {
//     // m->length + 4 est dû au fait que le contenu du message commence au 5e
//     // octet.
//     tlv_to_char_array(m->body[i], addr, &ptr, m->length + 4);
//     printf("added ! %ld\n", ptr);
//   }
//   return 0;
// }

// // TODO ! SIZE il faut le calculer !
// size_t char_array_to_msg(char* s, msg_t** addr) {
//   size_t size = 0;
//   unsigned long ptr = 0;
//   *addr = malloc(sizeof(msg_t));
//   (*addr)->magic = s[ptr++];
//   (*addr)->version = s[ptr++];
//   (*addr)->length = s[ptr++] << 8;
//   (*addr)->length = s[ptr++] + (*addr)->length;
//   // TODO TLV
//   tlv_t** temp_ts = malloc(sizeof(tlv_t*));
//   tlv_t** temp_ts2 = malloc(sizeof(tlv_t*) * MSG_TLV_NB_DEF);
//   int nb = 0;
//   // to remove.
//   printf("Length : %d\n", (*addr)->length);
//   for (; ptr < (*addr)->length - 3; nb++) {
//     printf("ptr: %ld\n", ptr);
//     char_array_to_tlv(s, temp_ts, &ptr, (size_t)(*addr)->length + 4);
//     temp_ts2[nb] = *temp_ts;
//     print_tlv(*temp_ts);
//   }

//   (*addr)->body = malloc(sizeof(tlv_t*) * nb);
//   for (int i = 0; i < nb; i++) {
//     (*addr)->body[i] = temp_ts2[i];
//   }

//   // (*addr)->body = temp_ts2;
//   (*addr)->tlv_nb = nb;

//   return size;
// }

// size_t tlv_to_char_array(tlv_t* t,
//                          char** addr,
//                          unsigned long* ptr,
//                          size_t max_ptr) {
//   (*addr)[(*ptr)++] = t->type;
//   (*addr)[(*ptr)++] = t->length;

//   switch (t->type) {
//     case TLV_PAD1:
//       return 0;
//       break;
//     case TLV_PADN:
//       memset((*addr + *ptr), 0, t->body.pad_n.zeroes);
//       *ptr += t->body.pad_n.zeroes;
//       break;
//     case TLV_HELLO:
//       memcpy((*addr + *ptr), &(t->body.hello.source_id), sizeof(uint64_t));
//       *ptr += sizeof(uint64_t);
//       if (t->body.hello.type) {
//         memcpy((*addr + *ptr), &(t->body.hello.dest_id), sizeof(uint64_t));
//         *ptr += sizeof(uint64_t);
//       }
//       break;
//     case TLV_NEIGHBOUR:
//       memcpy((*addr + *ptr), (t->body.neighbour.ip),
//              sizeof(unsigned char) * 16);
//       *ptr += sizeof(unsigned char) * 16;
//       memcpy((*addr + *ptr), &(t->body.neighbour.port), sizeof(uint16_t));
//       *ptr += sizeof(uint16_t);
//       break;
//     case TLV_DATA:
//       // TODO
//       memcpy((*addr + *ptr), &(t->body.data), t->length);
//       *ptr += sizeof(struct data);
//       break;
//     case TLV_ACK:
//       memcpy((*addr + *ptr), &(t->body.ack), t->length);
//       *ptr += t->length;
//       break;
//     case TLV_GO_AWAY:
//       // TODO
//       memcpy((*addr + *ptr), &(t->body.go_away), t->length);
//       break;
//     case TLV_WARNING:
//       // TODO
//       memcpy((*addr + *ptr), &(t->body.warning), t->length);
//       break;
//     default:
//       return 0;
//       break;
//   }

//   // TODO
//   return t->length;
// }

// size_t char_array_to_tlv(char* s,
//                          tlv_t** addr,
//                          unsigned long* ptr,
//                          size_t max_ptr) {
//   *addr = malloc(sizeof(tlv_t*));
//   (*addr)->type = s[(*ptr)++];
//   (*addr)->length = s[(*ptr)++];

//   switch ((*addr)->type) {
//     case TLV_PAD1:
//       break;
//     case TLV_PADN:
//       (*addr)->body.pad_n.zeroes = (unsigned int)(*addr)->length;
//       *ptr += (unsigned long)(*addr)->body.pad_n.zeroes;
//       break;
//     case TLV_HELLO:
//       (*addr)->body.hello.type = (*addr)->length / sizeof(uint64_t) == 2;
//       char_to_member(s, ptr, &((*addr)->body.hello.source_id),
//                      sizeof(uint64_t));
//       if ((*addr)->body.hello.type == LONG_HELLO)
//         char_to_member(s, ptr, &((*addr)->body.hello.dest_id),
//                        sizeof(uint64_t));
//       break;
//     case TLV_NEIGHBOUR:
//       char_to_member(s, ptr, (*addr)->body.neighbour.ip,
//                      sizeof(unsigned char) * 16);
//       char_to_member(s, ptr, &((*addr)->body.neighbour.port),
//       sizeof(uint16_t)); break;
//     case TLV_DATA:
//       // TODO
//       memcpy(&((*addr)->body.data), (s + *ptr), (*addr)->length);
//       break;
//     case TLV_ACK:
//       memcpy(&((*addr)->body.data), (s + *ptr), (*addr)->length);
//       *ptr += (*addr)->length;
//       break;
//     case TLV_GO_AWAY:
//       // TODO
//       memcpy(&((*addr)->body.go_away), (s + *ptr), (*addr)->length);
//       break;
//     case TLV_WARNING:
//       // TODO
//       memcpy(&((*addr)->body.warning), (s + *ptr), (*addr)->length);
//       break;
//     default:
//       // TODO : implement error.
//       break;
//   }

//   // TODO
//   return 0;
// }

// size_t char_to_member(char* s, unsigned long* ptr, void* member, size_t size)
// {
//   memcpy(member, (s + *ptr), size);
//   (*ptr) += size;
//   return size;
// }

// short ack_from_data(tlv_t* data_tlv, tlv_t** ack_tlv) {
//   if (data_tlv->type != TLV_DATA)
//     return 0;
//   *ack_tlv =
//       new_tlv_ack(data_tlv->body.data.sender_id, data_tlv->body.data.nonce);
//   return (short)1;
// }
