/**
 * @file types.h
 * @author Félix Desmaretz
 * @brief File to store all custom types, their constructors/generators and free
 * functions.
 * @version 0.1
 * @date 2019-04-02
 *
 * @copyright Copyright (c) 2019
 *
 */

#ifndef TYPES_H_
#define TYPES_H_

#include <inttypes.h>
#include <netdb.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>

#include "map.h"

#define MSG_SIZE 4096
#define MSG_MAGIC 93
#define MSG_VERSION 2
#define MSG_TLV_NB_DEF 64

typedef enum { DATA_VISIBLE = 0, DATA_HIDDEN = 1 } DATA_TYPE;

typedef enum {
  TLV_UNDEFINED = -1,
  TLV_PAD1 = 0,
  TLV_PADN = 1,
  TLV_HELLO = 2,
  TLV_NEIGHBOUR = 3,
  TLV_DATA = 4,
  TLV_ACK = 5,
  TLV_GO_AWAY = 6,
  TLV_WARNING = 7
} TLV_TYPE;

typedef struct pad_n {
  unsigned int zeroes;
} padn_body_t;

typedef struct hello {
  bool is_long;
  uint64_t source_id;
  uint64_t dest_id;
} hello_body_t;

typedef struct neighbour_body {
  unsigned char ip[16];
  uint16_t port;
} neighbour_body_t;

typedef struct data {
  uint64_t sender_id;
  uint32_t nonce;
  unsigned char type;
  size_t data_len;
  char* data;
} data_body_t;

typedef struct ack {
  uint64_t sender_id;
  uint32_t nonce;
} ack_body_t;

typedef struct go_away {
  char code;
  bool has_message;
  char* message;
} go_away_body_t;

typedef struct warning {
  size_t msg_len;
  char* message;
} warning_body_t;

typedef struct tlv_t {
  unsigned char type;
  unsigned char length;
  void* body;
} tlv_t;

typedef struct msg_t {
  unsigned char magic;
  unsigned char version;
  uint16_t length;
  size_t tlv_nb;
  tlv_t** body;
} msg_t;

/**
 * @brief Generates a new padn_body_t with n zeroes.
 *
 * @param n Number of zeroes.
 * @return padn_body_t* New padn_body_t
 */
padn_body_t* new_padn_body(unsigned int n);

/**
 * @brief Generates a new hello_body_t.
 *
 * @param is_long If true dst_id will be considered.
 * @param src_id Source ID.
 * @param dst_id Destination ID.
 * @return hello_body_t* New hello_body_t.
 */
hello_body_t* new_hello_body(bool is_long, uint64_t src_id, uint64_t dst_id);

/**
 * @brief Generates a new neighbour_body_t.
 *
 * @param ip IP.
 * @param port Port.
 * @return neighbour_body_t* New neighbour_body_t.
 */
neighbour_body_t* new_neighbour_body(unsigned char* ip, uint16_t port);

/**
 * @brief Generates a new data_body_t
 *
 * @param sender_id Sender ID.
 * @param nonce Nonce.
 * @param type The type will be implemented further on... (TODO)
 * @param data String of data.
 * @param data_len Length of the string.
 * @return data_body_t* New data_body_t.
 */
data_body_t* new_data_body(uint64_t sender_id,
                           uint32_t nonce,
                           unsigned char type,
                           char* data,
                           size_t data_len);

/**
 * @brief Genereates a new ack_body_t
 *
 * @param sender_id Sender ID.
 * @param nonce Nonce.
 * @return ack_body_t* New ack_body_t
 */
ack_body_t* new_ack_body(uint64_t sender_id, uint32_t nonce);

/**
 * @brief Generates a new go_away_body_t.
 *
 * @param code Code of the GO_AWAY.
 * @param m Message.
 * @param msg_len Message length.
 * @return go_away_body_t* New go_away_body_t.
 */
go_away_body_t* new_go_away_body(char code, char* m, size_t msg_len);

/**
 * @brief Generates a new warning_body_t.
 *
 * @param msg Message.
 * @param msg_len Message length.
 * @return warning_body_t* New warning_body_t.
 */
warning_body_t* new_warning_body(char* msg, size_t msg_len);

/**
 * @brief Generates a TLV to be ignored at reception with an empty body.
 *
 * @return tlv_t* Pad1.
 */
tlv_t* new_tlv_pad1();

/**
 * @brief Generates a TLV to be ignored at reception with a body of n octets set
 * at 0.
 *
 * @param n Number of bytes.
 * @return tlv_t* PadN.
 */
tlv_t* new_tlv_padn(unsigned int n);

/**
 * @brief Generates a Hello TLV.
 *
 * A SHORT hello of 8 bytes is to be sent to a potential pair of which we don't
 * know the ID. The LONG hello of 16 bytes will be sent back with the
 * destination ID included.
 *
 * @param is_long If true then dest_id will be considered.
 * @param source_id
 * @param dest_id To be specified if type is LONG_HELLO, else it will be
 * ignored.
 * @return tlv_t* Hello TLV.
 */
tlv_t* new_tlv_hello(bool is_long, uint64_t source_id, uint64_t dest_id);

/**
 * @brief Generates a Neighbour TLV
 *
 * @param ip IP address of the pair.
 * @param port Port of the pair.
 * @return tlv_t* Neighbour TLV
 */
tlv_t* new_tlv_neighbour(unsigned char* ip, uint16_t port);

/**
 * @brief Generates a Data TLV.
 *
 * @param sender_id Id of the original sender of the message.
 * @param nonce 32 bit chain that makes the pair (Sender ID, Nonce) globally
 * unique.
 * @param type Can be VISIBLE or HIDDEN. If VISIBLE, it will be writen in the
 * chat window of the clients. Other types can be implemented further down the
 * road.
 * @param data Data/Message to send.
 * @param data_len Data length.
 * @return tlv_t* Data TLV.
 */
tlv_t* new_tlv_data(uint64_t sender_id,
                    DATA_TYPE type,
                    char* data,
                    size_t data_len);

/**
 * @brief Generates an Ack TLV.
 *
 * @param sender_id ID of the Ack creator.
 * @param nonce Nonce of teh corresponding Data TLV.
 * @return tlv_t* Ack TLV.
 */
tlv_t* new_tlv_ack(uint64_t sender_id, uint32_t nonce);

/**
 * @brief Generates a GoAway TLV.
 *
 * @param code
 *  0 : Reason Unknown\n
 *  1 : The emitter quits the network.\n
 *  2 : The receptor did not send a Hello ina long time or has not sent an Ack
 * to a Data TLV in a long time.\n
 *  3 : The receptor violated the protocol.
 * @param message Debug message to send.
 * @param message_len Length of the message.
 * @return tlv_t* GoAway TLV.
 */
tlv_t* new_tlv_go_away(char code, char* message, size_t message_len);

/**
 * @brief Generates a Warning TLV.
 *
 * @param message Warning message.
 * @param message_len Length of the message;
 * @return tlv_t* Warning TLV.
 */
tlv_t* new_tlv_warning(char* message, size_t message_len);

/**
 * @brief Generates a message.
 *
 * @param ts Array of TLV.
 * @param ts_size Size of the array.
 * @return msg_t* Message.
 */
msg_t* new_msg(tlv_t** ts, size_t ts_size);

/**
 * @brief Generates a nonce from a user ID, using time.
 *
 * @param id
 * @return uint32_t
 */
uint32_t new_nonce(uint64_t id);

void free_padn_body(padn_body_t* b);
void free_hello_body(hello_body_t* b);
void free_neighbour_body(neighbour_body_t* b);
void free_data_body(data_body_t* b);
void free_ack_body(ack_body_t* b);
void free_go_away_body(go_away_body_t* b);
void free_warning_body(warning_body_t* b);
void free_tlv(tlv_t* t);
void free_msg(msg_t* t, bool free_tlv);

/**
 * ####################################
 * SERIALIZATION
 * ####################################
 */

#define SBUFF_INIT_SIZE 64
#define DBUFF_INIT_SIZE 2 * SBUFF_INIT_SIZE

/**
 * @brief Serial Buffer
 *
 */

typedef struct sbuff_t {
  size_t size;
  size_t next;
  size_t real_size;
  void* data;
} sbuff_t;

/**
 * @brief Creates a new buffer.
 *
 * @return sbuff_t* buffer.
 */
sbuff_t* new_sbuff();

/**
 * @brief Creates a new buffer du deserialize.
 *
 * @param data Data
 * @param data_len Length of the data
 * @return sbuff_t* buffer.
 */
sbuff_t* new_dsbuff(unsigned char* data, size_t data_len);

/**
 * @brief Allocates space for the buffer data.
 *
 * @param b Buffer
 * @param bytes Bytes to reserve.
 */
void sbuff_reserve_space(sbuff_t* b, size_t bytes);

void free_sbuff(sbuff_t* b);

/**
 * ####################
 * NEIGHBOUR MAP
 * ####################
 */

/**
 * @brief neighbour entry for the neighbour_map_t.
 *
 */
typedef struct neighbour_entry_t {
  struct addrinfo* addr;
  time_t last_short_hello;
  time_t last_long_hello;
} neighbour_entry_t;

typedef map_t(neighbour_entry_t*) neighbour_map_t;

/**
 * @brief Generates a neighbour_entry_t with set ip and port and last and long
 * hello at 0.
 *
 * @param ip IP
 * @param port Port
 * @return neighbour_entry_t* Generated entry.
 */
neighbour_entry_t* new_neighbour_entry(struct addrinfo* addr);

/**
 * @brief Frees all allocated ressources to a neighbour entry.
 *
 * @param e Entry to free.
 */
void free_neighbour_entry(neighbour_entry_t* e);

/**
 * @brief Generates an empty neighbour_map_t.
 *
 * @return neighbour_map_t* Map generated.
 */
neighbour_map_t* new_neighbour_map();

/**
 * @brief Generated the key for a given neighbour.
 *
 * @param ip IP.
 * @param port Port.
 * @return char* Key
 */
char* new_neighbour_key(char* ip, uint16_t port);

/**
 * ####################
 * DATA MAP
 * ####################
 */

typedef struct data_entry_t {
  data_body_t* body;
} data_entry_t;

typedef map_t(data_entry_t*) data_map_t;

char *new_data_key(uint64_t id, uint32_t nonce);

/**
 * ####################
 * DLLIST
 * ####################
 */

/**
 * @brief Neighbour type for dllist_t.
 *
 */
typedef struct dll_neighbour_t {
  char* map_key;
  uint8_t tries;
  time_t next_try;
} dll_neighbour_t;

dll_neighbour_t* new_dll_neighbour(neighbour_entry_t* b);
void free_dll_neighbour(dll_neighbour_t* n);

/**
 * @brief All the node types for the dllist_t, add a new type here when needed.
 *
 */
typedef enum { DLL_NEIGHBOUR, DLL_INT, DLL_MSG } DLL_NODE_TYPE;

/**
 * @brief Generic double linked list node.
 *
 */
typedef struct dllist_node_t {
  DLL_NODE_TYPE type;
  struct dllist_node_t* next;
  struct dllist_node_t* prev;
  void* data;
} dllist_node_t;

/**
 * @brief Generic double linked list.
 *
 */
typedef struct dllist_t {
  dllist_node_t* first;
  dllist_node_t* last;
  size_t size;
  DLL_NODE_TYPE type;
} dllist_t;

/**
 * @brief Msg type for dllist_t.
 *
 */
typedef struct dll_msg_t {
  data_body_t* data;
  dllist_t* neighbours;
} dll_msg_t;

dll_msg_t* new_dll_msg(data_body_t* data);
void free_dll_msg(dll_msg_t* m, bool data);

/**
 * @brief Generates a new dllist
 *
 * @param type type of the data that will be contained.
 * See DLL_NODE_TYPE.
 * @return dllist_t* New list.
 */
dllist_t* new_dllist(DLL_NODE_TYPE type);

/**
 * @brief Generates a new node.
 *
 * @param type Type of the data, see DLL_NODE_TYPE.
 * @param data Data to be contained.
 * @return dllist_node_t* new node.
 */
dllist_node_t* new_dllist_node(DLL_NODE_TYPE type, void* data);

/**
 * @brief Frees a list.
 *
 * @param list List to free.
 * @param erase_data 1 if data must be erased, else 0.
 */
void dllist_free(dllist_t* list, bool erase_data);

/**
 * @brief Frees a node.
 *
 * @param node Node to free.
 * @param erase_data 1 if data must be erased, else 0.
 */
void dllist_free_node(dllist_node_t* node, bool erase_data);

/**
 * @brief Checks if list is empty.
 *
 * @param list List.
 * @return short 1 if empty else 0.
 */
bool dllist_is_empty(dllist_t* list);

// dll_neighbour_t *new_dll_neighbour_from_entry(neighbour_entry_t *e);
dllist_t* new_neighbour_list(char* key_to_bypass);

#endif  // !TYPES_H_
