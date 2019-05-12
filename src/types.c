#include "types.h"

padn_body_t* new_padn_body(unsigned int n) {
  padn_body_t* b = malloc(sizeof(padn_body_t));
  b->zeroes = n;
  return b;
}

hello_body_t* new_hello_body(bool is_long, uint64_t src_id, uint64_t dst_id) {
  hello_body_t* b = malloc(sizeof(hello_body_t));
  b->source_id = src_id;
  b->dest_id = is_long ? dst_id : 0;
  b->is_long = is_long;
  return b;
}

neighbour_body_t* new_neighbour_body(unsigned char* ip, uint16_t port) {
  neighbour_body_t* b = malloc(sizeof(neighbour_body_t));

  // améliorer avec un memcpy
  for (int i = 0; i < 16; i++)
    b->ip[i] = *(ip + i);
  b->port = port;
  return b;
}

data_body_t* new_data_body(uint64_t sender_id,
                           uint32_t nonce,
                           unsigned char type,
                           char* data,
                           size_t data_len) {
  data_body_t* b = malloc(sizeof(data_body_t));
  b->sender_id = sender_id;
  b->nonce = nonce;
  b->type = type;
  b->data_len = data_len;

  // TODO ajouter la taille max;
  // b->data = malloc(sizeof(char) * data_len);

  // // améliorer avec memcpy...
  // for (int i = 0; i < data_len; i++)
  //   *(b->data + i) = *(data + i);

  b->data = data;

  return b;
}

ack_body_t* new_ack_body(uint64_t sender_id, uint32_t nonce) {
  ack_body_t* b = malloc(sizeof(ack_body_t));
  b->sender_id = sender_id;
  b->nonce = nonce;
  return b;
}

go_away_body_t* new_go_away_body(char code, char* m, size_t msg_len) {
  go_away_body_t* b = malloc(sizeof(go_away_body_t));
  b->code = code;
  b->has_message = msg_len > 0;
  if (b->has_message) {
    // b->message = malloc(sizeof(char) * msg_len);
    // for (int i = 0; i < 0; i++)
    //   *(b->message + i) = *(m + i);
    b->message = m;
  }

  return b;
}

warning_body_t* new_warning_body(char* msg, size_t msg_len) {
  warning_body_t* b = malloc(sizeof(warning_body_t));
  // for (int i = 0; i < 0; i ++)
  //   *(b->message + i) + *(msg + i);
  b->msg_len = msg_len;
  b->message = msg;
  return b;
}

tlv_t* new_tlv_pad1() {
  tlv_t* out = malloc(sizeof(tlv_t));
  out->type = (unsigned char)TLV_PAD1;
  out->length = 0;
  return out;
}

tlv_t* new_tlv_padn(unsigned int n) {
  tlv_t* out = malloc(sizeof(tlv_t));
  out->type = (unsigned char)TLV_PADN;
  out->length = n;
  out->body = new_padn_body(n);
  return out;
}

tlv_t* new_tlv_hello(bool is_long, uint64_t source_id, uint64_t dest_id) {
  tlv_t* out = malloc(sizeof(tlv_t));
  out->type = (unsigned char)TLV_HELLO;
  out->length = sizeof(uint64_t) + (is_long ? sizeof(uint64_t) : 0);
  out->body = new_hello_body(is_long, source_id, dest_id);
  return out;
}

tlv_t* new_tlv_neighbour(unsigned char* ip, uint16_t port) {
  tlv_t* out = malloc(sizeof(tlv_t));
  out->type = (unsigned char)TLV_NEIGHBOUR;
  out->length = sizeof(char) * 8 + sizeof(uint16_t);
  out->body = new_neighbour_body(ip, port);
  return out;
}

// TODO: Implémenter length.
tlv_t* new_tlv_data(uint64_t sender_id,
                    DATA_TYPE type,
                    char* data,
                    size_t data_len) {
  tlv_t* out = malloc(sizeof(tlv_t));
  out->type = (unsigned char)TLV_DATA;
  out->length = sizeof(char) * data_len + sizeof(uint64_t) + sizeof(uint32_t) +
                sizeof(char);
  out->body =
      new_data_body(sender_id, new_nonce(sender_id), type, data, data_len);
  return out;
}

tlv_t *new_tlv_data_b(data_body_t *body) {
  tlv_t* out = malloc(sizeof(tlv_t));
  out->type = (unsigned char)TLV_DATA;
  out->length = sizeof(char) * body->data_len + sizeof(uint64_t) + sizeof(uint32_t) +
                sizeof(char);
  out->body = body;
  return out;
}

tlv_t* new_tlv_ack(uint64_t sender_id, uint32_t nonce) {
  tlv_t* out = malloc(sizeof(tlv_t));
  out->type = (unsigned char)TLV_ACK;
  out->length = sizeof(uint64_t) + sizeof(uint32_t);
  out->body = new_ack_body(sender_id, nonce);
  return out;
}

tlv_t* new_tlv_go_away(char code, char* message, size_t message_len) {
  tlv_t* out = malloc(sizeof(tlv_t));
  out->type = (unsigned char)TLV_GO_AWAY;
  out->length = sizeof(char);
  out->body = new_go_away_body(code, message, message_len);
  return out;
}

tlv_t* new_tlv_warning(char* message, size_t message_len) {
  tlv_t* out = malloc(sizeof(tlv_t));
  out->type = (unsigned char)TLV_WARNING;
  out->length = message_len;
  out->body = new_warning_body(message, message_len);
  return out;
}

msg_t* new_msg(tlv_t** ts, size_t ts_size) {
  msg_t* m = malloc(sizeof(msg_t));
  m->magic = MSG_MAGIC;
  m->version = MSG_VERSION;
  m->tlv_nb = ts_size;
  // TODO: Gérer la taille maximale d'un message.
  m->body = ts;
  m->length = 0;
  for (int i = 0; i < ts_size; i++) {
    m->length += sizeof(char) * 2 + ts[i]->length;
  }
  return m;
}

uint32_t new_nonce(uint64_t id) {
  time_t s;
  time(&s);

  return (s ^ 2) / ((uint32_t)id);
}

void free_padn_body(padn_body_t* b) {
  free(b);
}

void free_hello_body(hello_body_t* b) {
  free(b);
}

void free_neighbour_body(neighbour_body_t* b) {
  free(b);
}

void free_data_body(data_body_t* b) {
  free(b->data);
  free(b);
}

void free_ack_body(ack_body_t* b) {
  free(b);
}

void free_go_away_body(go_away_body_t* b) {
  if (b->has_message)
    free(b->message);
  free(b);
}

void free_warning_body(warning_body_t* b) {
  if (b->msg_len > 0)
    free(b->message);
  free(b);
}

void free_tlv(tlv_t* t) {
  switch (t->type) {
    case TLV_PAD1:
      break;
    case TLV_PADN:
      free_padn_body(t->body);
      break;
    case TLV_HELLO:
      free_hello_body(t->body);
      break;
    case TLV_NEIGHBOUR:
      free_hello_body(t->body);
      break;
    case TLV_DATA:
      free_data_body(t->body);
      break;
    case TLV_ACK:
      free_ack_body(t->body);
      break;
    case TLV_GO_AWAY:
      free_go_away_body(t->body);
      break;
    case TLV_WARNING:
      free_warning_body(t->body);
      break;
    default:
      break;
  }

  free(t);
}

void free_msg(msg_t* m, bool tlv) {
  if (tlv) {
    for (int i = 0; i < m->tlv_nb; i++)
      free_tlv(m->body[i]);
  }
  free(m->body);
  free(m);
}

/**
 * ####################################
 * SERIALIZATION
 * ####################################
 */

sbuff_t* new_sbuff() {
  sbuff_t* b = malloc(sizeof(sbuff_t));

  b->data = malloc(SBUFF_INIT_SIZE);
  b->size = SBUFF_INIT_SIZE;
  b->next = 0;
  b->real_size = 0;

  return b;
}

sbuff_t* new_dsbuff(unsigned char* data, size_t data_len) {
  sbuff_t* b = malloc(sizeof(sbuff_t));

  b->data = data;
  b->size = data_len;
  b->next = 0;
  b->real_size = data_len;

  printf("Buffer size: %ld\n", b->real_size);

  return b;
}

void sbuff_reserve_space(sbuff_t* b, size_t bytes) {
  while ((b->next + bytes) > b->size) {
    b->data = realloc(b->data, b->size * 2);
    b->size *= 2;
  }
}

void free_sbuff(sbuff_t* b) {
  free(b->data);
  free(b);
}

/**
 * ####################
 * DLLIST
 * ####################
 */

time_t set_time_send(uint16_t tries) {
  uint64_t min, max;
  min = (uint64_t)pow(2, (double)tries);
  max = (uint64_t)pow(2, (double)tries + 1);
  uint64_t r = random() % (max - min) + min;
  return time(NULL) + r;
}

dllist_msg_t* new_dllist_msg(char* key, data_body_t* body) {
  dllist_msg_t* e = malloc(sizeof(dllist_msg_t));
  e->tries = 0;
  e->map_key = key;
  e->time_send = set_time_send(e->tries);
  e->body = body;
  return e;
}
void free_dllist_msg(dllist_msg_t* m, bool data) {
  if (data)
    free_data_body(m->body);
  free(m);
}

dll_neighbour_t* new_dll_neighbour(neighbour_entry_t* b, char* key) {
  dll_neighbour_t* n = malloc(sizeof(dll_neighbour_t));
  n->map_key = key;
  n->tries = 0;
  return n;
}

void free_dll_neighbour(dll_neighbour_t* n) {
  // free(n->ip);
  free(n);
}

void dllist_free(dllist_t* list, bool erase_data) {
  if (dllist_is_empty(list))
    return;
  dllist_node_t* n = list->first;
  while ((n = n->next)) {
    dllist_free_node(n->prev, erase_data);
  }
  dllist_free_node(n, erase_data);
}

void dllist_free_node(dllist_node_t* node, bool erase_data) {
  node->next = NULL;
  node->prev = NULL;
  if (erase_data) {
    if (node->type == DLL_STRING || node->type == DLL_INT)
      free(node->data);
    // TODO ajouter les destructeurs nécessaires en fonction du type de donnée.
    printf("node freed\n");
    free(node->data);
  }

  free(node);
}

dllist_t* new_dllist(DLL_NODE_TYPE type) {
  dllist_t* list = malloc(sizeof(dllist_t));
  list->size = 0;
  list->first = NULL;
  list->last = NULL;
  list->type = type;
  return list;
}

dllist_node_t* new_dllist_node(DLL_NODE_TYPE type, void* data) {
  dllist_node_t* node = malloc(sizeof(dllist_node_t));
  node->data = data;
  node->type = type;
  return node;
}

bool dllist_is_empty(dllist_t* list) {
  return list->first == NULL || list->last == NULL;
}

/**
 * ####################
 * NEIGHBOUR MAP
 * ####################
 */

neighbour_entry_t* new_neighbour_entry(struct addrinfo* addr) {
  neighbour_entry_t* e = malloc(sizeof(neighbour_entry_t));
  e->addr = malloc(sizeof(struct addrinfo));
  e->addr = addr;
  e->last_short_hello = 0;
  e->last_long_hello = 0;
  e->pmtu = 1024;
  e->msg_to_send = malloc(sizeof(dllist_t));
  return e;
}


void free_neighbour_entry(neighbour_entry_t* e) {
  freeaddrinfo(e->addr);
  // TODO free dllist
  free(e);
}

neighbour_map_t* new_neighbour_map() {
  neighbour_map_t* m = malloc(sizeof(neighbour_map_t));
  map_init(m);
  return m;
}

char* new_neighbour_key(char* ip, uint16_t port) {
  size_t len = INET6_ADDRSTRLEN + sizeof(port) + 1;
  char* key = malloc(sizeof(char) * len);
  snprintf(key, len, "%s%u", ip, port);
  return key;  // Il faut free à chaque usage.
}

char* new_neighbour_key_sock(struct sockaddr_in6* s) {
  char str_ip[INET6_ADDRSTRLEN];
  inet_ntop(AF_INET6, &s->sin6_addr, str_ip, INET6_ADDRSTRLEN);
  return new_neighbour_key(str_ip, s->sin6_port);
}

// dll_neighbour_t* new_dll_neighbour_from_entry(neighbour_entry_t* e) {
//   return new_dll_neighbour((struct sockaddr_in6*)e->addr->ai_addr);
// }

/**
 * ####################
 * DATA MAP
 * ####################
 */

data_map_t* new_data_map() {
  data_map_t* m = malloc(sizeof(data_map_t));
  map_init(m);
  return m;
}

char* new_data_key(uint64_t id, uint32_t nonce) {
  char* key = malloc(sizeof(char) * 20);
  snprintf(key, 20, "%ld%d", id, nonce);
  return key;
}