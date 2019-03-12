/**
 * @file msg.h
 * @author Félix Desmaretz
 * @brief
 * @version 0.1
 * @date 2019-03-11
 *
 * @copyright Copyright (c) 2019
 *
 */
#define _GNU_SOURCE

#include <inttypes.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define MSG_SIZE 4096
#define MSG_MAGIC 93
#define MSG_VERSION 2

typedef enum { SHORT_HELLO = 0, LONG_HELLO = 1 } HELLO_TYPE;

typedef enum { VISIBLE = 0, HIDDEN = 1 } DATA_TYPE;

typedef enum {
  PAD1 = 0,
  PADN = 1,
  HELLO = 2,
  NEIGHBOUR = 3,
  DATA = 4,
  ACK = 5,
  GO_AWAY = 6,
  WARNING = 7
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
      char *ip;
      uint16_t port;
    } neighbour;
    struct data {
      uint64_t sender_id;
      uint32_t nonce;
      unsigned char type;
      char *data;
    } data;
    struct ack {
      uint64_t sender_id;
      uint32_t nonce;
    } ack;
    struct go_away {
      char code;
      char *message;
    } go_away;
    struct warning {
      char *message;
    } warning;
  } body;
} tlv_t;

typedef struct {
  unsigned char magic;
  unsigned char version;
  uint16_t length;
  tlv_t **body;
  size_t tlv_nb;
} msg_t;

/**
 * @brief Generates a TLV to be ignored at reception with an empty body.
 *
 * @return tlv_t* Pad1.
 */
tlv_t *gen_tlv_pad1();

/**
 * @brief Generates a TLV to be ignored at reception with a body of n octets set
 * at 0.
 *
 * @param n Number of bytes.
 * @return tlv_t* PadN.
 */
tlv_t *gen_tlv_padn(unsigned int n);

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
tlv_t *gen_tlv_hello(HELLO_TYPE type, uint64_t source_id, uint64_t dest_id);

/**
 * @brief Generates a Neighbour TLV
 *
 * @param ip IP address of the pair.
 * @param port Port of the pair.
 * @return tlv_t* Neighbour TLV
 */
tlv_t *gen_tlv_neighbour(char *ip, uint16_t port);

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
tlv_t *gen_tlv_data(uint64_t sender_id, uint32_t nonce, DATA_TYPE type,
                    char *data);

/**
 * @brief Generates an Ack TLV.
 *
 * @param sender_id ID of the Ack creator.
 * @param nonce Nonce of teh corresponding Data TLV.
 * @return tlv_t* Ack TLV.
 */
tlv_t *gen_tlv_ack(uint64_t sender_id, uint32_t nonce);

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
tlv_t *gen_tlv_go_away(char code, short contains_message, char *message,
                       size_t message_len);

/**
 * @brief Generates a Warning TLV.
 *
 * @param message Warning message.
 * @param message_len Length of the message;
 * @return tlv_t* Warning TLV.
 */
tlv_t *gen_tlv_warning(char *message, size_t message_len);

/**
 * @brief Generates a message.
 *
 * @param ts Array of TLV.
 * @param ts_size Size of the array.
 * @return msg_t* Message.
 */
msg_t *gen_msg(tlv_t **ts, size_t ts_size);

/**
 * @brief Prints the TLV displaying all useful informations.
 *
 * @param t TLV to print.
 */
void print_tlv(tlv_t *t);

/**
 * @brief Prints the message displaying allthe useful informations.
 *
 * @param m Message to print.
 */
void print_msg(msg_t *m);