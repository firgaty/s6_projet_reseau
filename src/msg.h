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

#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define MSG_SIZE 4096

typedef enum { SHORT_HELLO = 0, LONG_HELLO } HELLO_TYPE;

typedef enum { VISIBLE = 0, HIDDEN } DATA_TYPE;

typedef enum {
    PAD1 = 0,
    PADN,
    HELLO,
    NEIGHBOUR,
    DATA,
    ACK,
    GO_AWAY,
    WARNING
} TLV_TYPE;

typedef struct {
  unsigned char magic;
  unsigned char version;
  unsigned short length;
  char *body;
} msg_t;

typedef struct {
  unsigned char type;
  unsigned char length;
  union {
    struct pad_n {
      unsigned int zeroes;
    } pad_n;
    struct hello {
      unsigned char type;
      unsigned long long source_id;
      unsigned long long dest_id;
    } hello;
    struct neighbour {
      char *ip;
      unsigned int port;
    } neighbour;
    struct data {
      unsigned long long sender_id;
      unsigned long nonce;
      unsigned char type;
      char *data;
    } data;
    struct ack {
      unsigned long long sender_id;
      unsigned long nonce;
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
tlv_t *gen_tlv_hello(HELLO_TYPE type, unsigned long long source_id,
                     unsigned long long dest_id);

/**
 * @brief Generates a Neighbour TLV
 *
 * @param ip IP address of the pair.
 * @param port Port of the pair.
 * @return tlv_t* Neighbour TLV
 */
tlv_t *gen_tlv_neighbour(char *ip, unsigned int port);

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
tlv_t *gen_tlv_data(unsigned long long sender_id,
                    DATA_TYPE type, char *data);

/**
 * @brief Generates an Ack TLV.
 *
 * @param sender_id ID of the Ack creator.
 * @param nonce Nonce of teh corresponding Data TLV.
 * @return tlv_t* Ack TLV.
 */
tlv_t *gen_tlv_ack(unsigned long long sender_id, unsigned long nonce);

/**
 * @brief Generates a GoAway TLV
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
 * @return tlv_t*
 */
tlv_t *gen_tlv_go_away(char code, short contains_message, char *message,
                       size_t message_len);

tlv_t *gen_tlv_warning(char *message, size_t message_len);

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
void pritn_msg(msg_t *m);