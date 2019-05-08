#include "serialization.h"

#define htonll(x)                                                  \
  ((1 == htonl(1)) ? (x)                                           \
                   : (((uint64_t)htonl((x)&0xFFFFFFFFUL)) << 32) | \
                         htonl((uint32_t)((x) >> 32)))
#define ntohll(x)                                                  \
  ((1 == ntohl(1)) ? (x)                                           \
                   : (((uint64_t)ntohl((x)&0xFFFFFFFFUL)) << 32) | \
                         ntohl((uint32_t)((x) >> 32)))

void serial_char(char c, sbuff_t* b) {
  sbuff_reserve_space(b, sizeof(char));
  memcpy(((char*)b->data) + b->next, &c, sizeof(char));
  b->next += sizeof(char);
  b->real_size += sizeof(char);
}

void serial_short(uint16_t s, sbuff_t* b) {
  s = htons(s);
  sbuff_reserve_space(b, sizeof(uint16_t));
  memcpy(((char*)b->data) + b->next, &s, sizeof(uint16_t));
  b->next += sizeof(uint16_t);
  b->real_size += sizeof(uint16_t);
}

void serial_int(int i, sbuff_t* b) {
  i = htonl(i);
  sbuff_reserve_space(b, sizeof(int));
  memcpy(((char*)b->data) + b->next, &i, sizeof(int));
  b->next += sizeof(int);
  b->real_size += sizeof(uint32_t);
}

void serial_long(uint64_t l, sbuff_t* b) {
  l = htonll(l);
  sbuff_reserve_space(b, sizeof(uint64_t));
  memcpy(((char*)b->data) + b->next, &l, sizeof(uint64_t));
  b->next += sizeof(uint64_t);
  b->real_size += sizeof(uint64_t);
}

void serial_str(char* s, size_t str_len, sbuff_t* b) {
  sbuff_reserve_space(b, sizeof(char) * str_len);
  memcpy(((char*)b->data) + b->next, &s, sizeof(char) * str_len);
  b->next += sizeof(char) * str_len;
  b->real_size += sizeof(char) * str_len;
}

// TODO
void serial_msg(msg_t* m, sbuff_t* b) {
  serial_char(m->magic, b);
  serial_char(m->version, b);
  serial_short(m->length, b);
  for (int i = 0; i < m->tlv_nb; i++)
    serial_tlv(m->body[i], b);
}

void serial_tlv(tlv_t* t, sbuff_t* b) {
  serial_char(t->type, b);
  serial_char(t->length, b);

  switch (t->type) {
    case TLV_PAD1:
      return;
      break;
    case TLV_PADN:
      serial_padn_body(t->body, b);
      break;
    case TLV_HELLO:
      serial_hello_body(t->body, b);
      break;
    case TLV_NEIGHBOUR:
      serial_neighbour_body(t->body, b);
      break;
    case TLV_DATA:
      serial_data_body(t->body, b);
      break;
    case TLV_ACK:
      serial_ack_body(t->body, b);
      break;
    case TLV_GO_AWAY:
      serial_go_away_body(t->body, b);
      break;
    case TLV_WARNING:
      serial_warning_body(t->body, b);
      break;
    default:
      break;
  }
}

void serial_padn_body(padn_body_t* e, sbuff_t* b) {
  for (int i = 0; i < e->zeroes; i++)
    serial_char('\0', b);
}

void serial_hello_body(hello_body_t* e, sbuff_t* b) {
  serial_long(e->source_id, b);
  if (e->is_long)
    serial_long(e->dest_id, b);
}

void serial_neighbour_body(neighbour_body_t* e, sbuff_t* b) {
  serial_str((char*)e->ip, 16, b);
  serial_short(e->port, b);
}

void serial_data_body(data_body_t* e, sbuff_t* b) {
  serial_long(e->sender_id, b);
  serial_int(e->nonce, b);
  serial_char(e->type, b);
  serial_str(e->data, e->data_len, b);
}

void serial_ack_body(ack_body_t* e, sbuff_t* b) {
  serial_long(e->sender_id, b);
  serial_int(e->nonce, b);
}

void serial_go_away_body(go_away_body_t* e, sbuff_t* b) {
  serial_char(e->code, b);
  if (e->has_message)
    serial_str(e->message, strlen(e->message), b);
}

void serial_warning_body(warning_body_t* e, sbuff_t* b) {
  serial_str(e->message, strlen(e->message), b);
}

//###############################################

char dserial_char(sbuff_t* b) {
  char c;
  memcpy(&c, ((char*)b->data) + b->next, sizeof(char));
  b->next += sizeof(char);
  return c;
}

uint16_t dserial_short(sbuff_t* b) {
  uint16_t s;
  memcpy(&s, ((char*)b->data) + b->next, sizeof(uint16_t));
  b->next += sizeof(uint16_t);
  s = ntohs(s);
  return s;
}

uint32_t dserial_int(sbuff_t* b) {
  uint32_t i;
  memcpy(&i, ((char*)b->data) + b->next, sizeof(int));
  b->next += sizeof(int);
  i = ntohl(i);
  return i;
}

uint64_t dserial_long(sbuff_t* b) {
  uint64_t l;
  memcpy(&l, ((char*)b->data) + b->next, sizeof(uint64_t));
  b->next += sizeof(uint64_t);
  l = ntohll(l);
  return l;
}

char* dserial_str(size_t str_len, sbuff_t* b) {
  printf("LENGTH: %ld\n", str_len);
  char* s = malloc(sizeof(char) * str_len);
  for (int i = 0; i < str_len; i ++) {
    s[i] = dserial_char(b);
  }
  // b->next += sizeof(char) * str_len;
  return s;
}

msg_t* dserial_msg(sbuff_t* b) {
  msg_t* m = malloc(sizeof(msg_t));
  m->magic = dserial_char(b);
  m->version = dserial_char(b);
  m->length = dserial_short(b);

  m->body = malloc(sizeof(tlv_t*) * MSG_TLV_NB_DEF);

  size_t count = 0;
  for (int i = 0; b->next < b->real_size; i++, count++)
    m->body[i] = dserial_tlv(b);

  m->tlv_nb = count;

  return m;
}

tlv_t* dserial_tlv(sbuff_t* b) {
  tlv_t* t = malloc(sizeof(tlv_t));
  t->type = (unsigned char)dserial_char(b);
  t->length = (unsigned char)dserial_char(b);

  printf("TLV: %u, %u\n", (uint16_t)t->type, (uint16_t)t->length);

  switch (t->type) {
    case TLV_PAD1:
      t->body = NULL;
      break;
    case TLV_PADN:
      printf("padn\n");
      t->body = dserial_padn_body(t->length, b);
      break;
    case TLV_HELLO:
      printf("Hello\n");
      t->body = dserial_hello_body(t->length, b);
      break;
    case TLV_NEIGHBOUR:
      printf("Neighbour\n");
      t->body = dserial_neighbour_body(t->length, b);
      break;
    case TLV_DATA:
      printf("DATA\n");
      t->body = dserial_data_body(t->length, b);
      break;
    case TLV_ACK:
      printf("ACK\n");
      t->body = dserial_ack_body(t->length, b);
      break;
    case TLV_GO_AWAY:
      printf("GoAWAY\n");
      t->body = dserial_go_away_body(t->length, b);
      break;
    case TLV_WARNING:
      printf("WARNING\n");
      t->body = dserial_warning_body(t->length, b);
      break;
    default:
      printf("Unknown TLV type.\n");
      t->body = NULL;
      break;
  }

  return t;
}

padn_body_t* dserial_padn_body(size_t tlv_len, sbuff_t* b) {
  for (int i = 0; i < tlv_len; i++)
    dserial_char(b);
  printf("%ld\n", b->next);
  return new_padn_body(tlv_len);
}

hello_body_t* dserial_hello_body(size_t tlv_len, sbuff_t* b) {
  bool is_long = tlv_len > 8;
  uint64_t src_id = dserial_long(b);
  uint64_t dst_id = is_long ? dserial_long(b) : 0;
  return new_hello_body(is_long, src_id, dst_id);
}

neighbour_body_t* dserial_neighbour_body(size_t tlv_len, sbuff_t* b) {
  unsigned char* ip =
      (unsigned char*)dserial_str(16, b);
  uint16_t port = dserial_short(b);
  neighbour_body_t* out = new_neighbour_body(ip, port);
  free(ip);
  return out;
}

data_body_t* dserial_data_body(size_t tlv_len, sbuff_t* b) {
  uint64_t sender_id = dserial_long(b);
  uint32_t nonce = dserial_int(b);
  unsigned char type = dserial_char(b);

  // TODO varier en fonction du type de data.
  size_t data_len =
      tlv_len - sizeof(uint64_t) - sizeof(uint32_t) - sizeof(char);
  char* data = dserial_str(data_len, b);
  return new_data_body(sender_id, nonce, type, data, data_len);
}

ack_body_t* dserial_ack_body(size_t tlv_len, sbuff_t* b) {
  uint64_t sender_id = dserial_long(b);
  uint32_t nonce = dserial_int(b);
  return new_ack_body(sender_id, nonce);
}

go_away_body_t* dserial_go_away_body(size_t tlv_len, sbuff_t* b) {
  char code = dserial_char(b);
  bool has_message = tlv_len > 1;
  if (has_message) {
    size_t len = tlv_len - sizeof(char);
    char* msg = dserial_str(len, b);
    return new_go_away_body(code, msg, len);
  }
  return new_go_away_body(code, NULL, 0);
}

warning_body_t* dserial_warning_body(size_t tlv_len, sbuff_t* b) {
  char* msg = dserial_str(tlv_len, b);
  return new_warning_body(msg, tlv_len);
}
