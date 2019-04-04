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

#ifndef TYPES_H
#define TYPES_H

#include <inttypes.h>
#include <time.h>

#define MSG_SIZE 4096
#define MSG_MAGIC 93
#define MSG_VERSION 2
#define MSG_TLV_NB_DEF 64

typedef enum { SHORT_HELLO = 0, LONG_HELLO = 1 } HELLO_TYPE;

typedef enum { VISIBLE = 0, HIDDEN = 1 } DATA_TYPE;

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

typedef struct tlv_t {
  unsigned char type;
  unsigned char length;
  union {
    struct pad_n {
      unsigned int zeroes;
    } pad_n;
    struct hello {
      unsigned char type;
      uint64_t source_id;
      uint64_t dest_id;
    } hello;
    struct neighbour {
      unsigned char* ip;
      uint16_t port;
    } neighbour;
    struct data {
      uint64_t sender_id;
      uint32_t nonce;
      unsigned char type;
      char* data;
    } data;
    struct ack {
      uint64_t sender_id;
      uint32_t nonce;
    } ack;
    struct go_away {
      char code;
      char* message;
    } go_away;
    struct warning {
      char* message;
    } warning;
  } body;
} tlv_t;

typedef struct msg_t {
  unsigned char magic;
  unsigned char version;
  uint16_t length;
  tlv_t** body;
  size_t tlv_nb;
} msg_t;

/**
 * @brief Generates a TLV to be ignored at reception with an empty body.
 *
 * @return tlv_t* Pad1.
 */
tlv_t* gen_tlv_pad1();

/**
 * @brief Generates a TLV to be ignored at reception with a body of n octets set
 * at 0.
 *
 * @param n Number of bytes.
 * @return tlv_t* PadN.
 */
tlv_t* gen_tlv_padn(unsigned int n);

/**
 * @brief Generates a Hello TLV.
 *
 * A SHORT hello of 8 bytes is to be sent to a potential pair of which we don't
 * know the ID. The LONG hello of 16 bytes will be sent back with the
 * destination ID included.
 *
 * @param type Can be SHORT_HELLO or LONG_HELLO.
 * @param source_id
 * @param dest_id To be specified if type is LONG_HELLO, else it will be
 * ignored.
 * @return tlv_t* Hello TLV.
 */
tlv_t* gen_tlv_hello(HELLO_TYPE type, uint64_t source_id, uint64_t dest_id);

/**
 * @brief Generates a Neighbour TLV
 *
 * @param ip IP address of the pair.
 * @param port Port of the pair.
 * @return tlv_t* Neighbour TLV
 */
tlv_t* gen_tlv_neighbour(unsigned char* ip, uint16_t port);

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
 * @return tlv_t* Data TLV.
 */
tlv_t* gen_tlv_data(uint64_t sender_id, DATA_TYPE type, char* data);

/**
 * @brief Generates an Ack TLV.
 *
 * @param sender_id ID of the Ack creator.
 * @param nonce Nonce of teh corresponding Data TLV.
 * @return tlv_t* Ack TLV.
 */
tlv_t* gen_tlv_ack(uint64_t sender_id, uint32_t nonce);

/**
 * @brief Generates a GoAway TLV.
 *
 * @param code
 *  0 : Reason Unknown\n
 *  1 : The emitter quits the network.\n
 *  2 : The receptor did not send a Hello ina long time or has not sent an Ack
 * to a Data TLV in a long time.\n
 *  3 : The receptor violated the protocol.
 * @param contains_message If 0, the message will be ignored.
 * @param message Debug message to send.
 * @param message_len Length of the message.
 * @return tlv_t* GoAway TLV.
 */
tlv_t* gen_tlv_go_away(char code,
                       short contains_message,
                       char* message,
                       size_t message_len);

/**
 * @brief Generates a Warning TLV.
 *
 * @param message Warning message.
 * @param message_len Length of the message;
 * @return tlv_t* Warning TLV.
 */
tlv_t* gen_tlv_warning(char* message, size_t message_len);

/**
 * @brief Generates a message.
 *
 * @param ts Array of TLV.
 * @param ts_size Size of the array.
 * @return msg_t* Message.
 */
msg_t* gen_msg(tlv_t** ts, size_t ts_size);

/**
 * ####################################
 * NEIGHBOUR
 * ####################################
 */

/**
 * @brief Neighbour type.
 *
 */
typedef struct neighbour_t {
  unsigned char* ip;
  uint16_t port;
  time_t last_hello;
  time_t last_hello_long;
} neighbour_t;

neighbour_t* gen_neighbour(unsigned char* ip, uint16_t port);
void free_neighbour(neighbour_t* n);

/**
 * ####################
 * DLLIST
 * ####################
 */

/**
 * @brief All the node types for the dllist_t, add a new type here when needed.
 *
 */
typedef enum { DLL_NEIGHBOUR, DLL_INT } DLL_NODE_TYPE;

/**
 * @brief Generic doubly linked list node.
 *
 */
typedef struct dllist_node_t {
  DLL_NODE_TYPE type;
  struct dllist_node_t* next;
  struct dllist_node_t* prev;
  void* data;
} dllist_node_t;

/**
 * @brief Generic doubly linked list.
 *
 */
typedef struct dllist_t {
  dllist_node_t* first;
  dllist_node_t* last;
  size_t size;
  DLL_NODE_TYPE type;
} dllist_t;

/**
 * @brief Generates a new dllist
 *
 * @param type type of the data that will be contained.
 * See DLL_NODE_TYPE.
 * @return dllist_t* New list.
 */
dllist_t* gen_dllist(DLL_NODE_TYPE type);

/**
 * @brief Generates a new node.
 *
 * @param type Type of the data, see DLL_NODE_TYPE.
 * @param data Data to be contained.
 * @return dllist_node_t* new node.
 */
dllist_node_t* gen_dllist_node(DLL_NODE_TYPE type, void* data);

/**
 * @brief Frees a list.
 *
 * @param list List to free.
 * @param erase_data 1 if data must be erased, else 0.
 */
void dllist_free(dllist_t* list, short erase_data);

/**
 * @brief Frees a node.
 *
 * @param node Node to free.
 * @param erase_data 1 if data must be erased, else 0.
 */
void dllist_free_node(dllist_node_t* node, short erase_data);

#endif  // !TYPES_H
