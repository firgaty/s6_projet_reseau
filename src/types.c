#include "types.h"


tlv_t* gen_tlv_pad1() {
  tlv_t* out = malloc(sizeof(tlv_t));
  out->type = (unsigned char)TLV_PAD1;
  out->length = 0;
  return out;
}

tlv_t* gen_tlv_padn(unsigned int n) {
  tlv_t* out = malloc(sizeof(tlv_t));
  out->type = (unsigned char)TLV_PADN;
  out->length = n;
  out->body.pad_n.zeroes = n;
  return out;
}

tlv_t* gen_tlv_hello(HELLO_TYPE type, uint64_t source_id, uint64_t dest_id) {
  tlv_t* out = malloc(sizeof(tlv_t));
  out->type = (unsigned char)TLV_HELLO;
  out->length = sizeof(uint64_t) + (type == LONG_HELLO ? sizeof(uint64_t) : 0);
  out->body.hello.source_id = source_id;
  out->body.hello.dest_id = type == LONG_HELLO ? dest_id : 0;
  out->body.hello.type = (unsigned char)type;
  return out;
}

tlv_t* gen_tlv_neighbour(unsigned char* ip, uint16_t port) {
  tlv_t* out = malloc(sizeof(tlv_t));
  out->type = (unsigned char)TLV_NEIGHBOUR;
  out->length = sizeof(char) * 8 + sizeof(uint16_t);
  out->body.neighbour.ip = ip;
  out->body.neighbour.port = port;
  return out;
}

// TODO: Implémenter length.
tlv_t* gen_tlv_data(uint64_t sender_id, DATA_TYPE type, char* data) {
  tlv_t* out = malloc(sizeof(tlv_t));
  out->type = (unsigned char)TLV_DATA;
  out->length = sizeof(char) * strlen(data) + sizeof(uint64_t) +
                sizeof(uint32_t) + sizeof(char);
  out->body.data.sender_id = sender_id;
  out->body.data.nonce = gen_nonce(sender_id);
  out->body.data.type = (unsigned char)type;
  out->body.data.data = data;
  return out;
}

tlv_t* gen_tlv_ack(uint64_t sender_id, uint32_t nonce) {
  tlv_t* out = malloc(sizeof(tlv_t));
  out->type = (unsigned char)TLV_ACK;
  out->length = sizeof(uint64_t) + sizeof(uint32_t);
  out->body.ack.nonce = nonce;
  out->body.ack.sender_id = sender_id;
  return out;
}

tlv_t* gen_tlv_go_away(char code,
                       short contains_message,
                       char* message,
                       size_t message_len) {
  tlv_t* out = malloc(sizeof(tlv_t));
  out->type = (unsigned char)TLV_GO_AWAY;
  out->length = sizeof(char);
  out->body.go_away.code = code;
  if (contains_message) {
    out->body.go_away.message = message;
    out->length += strlen(message);
  }
  return out;
}

tlv_t* gen_tlv_warning(char* message, size_t message_len) {
  tlv_t* out = malloc(sizeof(tlv_t));
  out->type = (unsigned char)TLV_WARNING;
  out->length = strlen(message);
  return out;
}

msg_t* gen_msg(tlv_t** ts, size_t ts_size) {
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

/**
 * ####################
 * NEIGHBOUR
 * #################### 
 */

/**
 * @brief Generates a new Neighbour.
 * 
 * @param char 
 * @param port 
 * @return neighbour_t* 
 */
neighbour_t* gen_neighbour(unsigned char* ip, uint16_t port) {
  neighbour_t* n = malloc(sizeof(neighbour_t));
  n->ip = ip;
  n->port = port;
  n->last_hello = 0;
  n->last_hello_long = 0;
}

void free_neighbour(neighbour_t* n) {
  free(n->ip);
  free(n);
}

/**
 * ####################
 * DLLIST
 * #################### 
 */



void dllist_free(dllist_t* list, short erase_data) {
  if (dllist_is_empty(list))
    return;
  dllist_node_t* n = list->first;
  while ((n = n->next)) {
    dllist_free_node(n->prev, erase_data);
  }
  dllist_free_node(n, erase_data);
}

void dllist_free_node(dllist_node_t* node, short erase_data) {
  node->next = NULL;
  node->prev = NULL;
  if (erase_data) {
    // TODO ajouter les destructeurs nécessaires en fonction du type de donnée.
    free(node->data);
  }

  free(node);
}

dllist_t* gen_dllist(DLL_NODE_TYPE type) {
  dllist_t* list = malloc(sizeof(dllist_t));
  list->size = 0;
  list->first = NULL;
  list->last = NULL;
  list->type = type;
  return list;
}

dllist_node_t* gen_dllist_node(DLL_NODE_TYPE type, void* data) {
  dllist_node_t* node = malloc(sizeof(dllist_node_t));
  node->data = data;
  node->type = type;
  return node;
}