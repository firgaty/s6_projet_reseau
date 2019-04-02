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
#ifndef MSG_H
#define MSG_H

#define _GNU_SOURCE

#include <inttypes.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#include "types.h"


/**
 * @brief Prints the TLV displaying all useful informations.
 *
 * @param t TLV to print.
 */
void print_tlv(tlv_t* t);

/**
 * @brief Prints the message displaying allthe useful informations.
 *
 * @param m Message to print.
 */
void print_msg(msg_t* m);

/**
 * @brief Transforms the message into a char array.
 *
 * It's the dual of char_array_to_msg()
 *
 * @param m Message to transform.
 * @param addr Address to store the array.
 * @return size_t Size of the created array.
 */
size_t msg_to_char_array(msg_t* m, char** addr);

/**
 * @brief Transforms the char array into a message.
 *
 * It's the dual of msg_to_char_array()
 *
 * @param s char array to transform.
 * @param addr Address to store the message.
 * @return size_t Number of TLV.
 */
size_t char_array_to_msg(char* s, msg_t** addr);

/**
 * @brief Transforms a TLV into a char array.
 *
 * @param t TLV to transform.
 * @param addr Addr to store the array.
 * @param ptr unsigned long that indicates where in the array to start to write.
 * @param max_ptr Determines the maximum size of the array stored.
 * @return size_t
 */
size_t tlv_to_char_array(tlv_t* t,
                         char** addr,
                         unsigned long* ptr,
                         size_t max_ptr);

/**
 * @brief Transforms a char array into a TLV.
 *
 * @param s char array to transform.
 * @param addr Address to store the TLV.
 * @param ptr unsigned long that indicates where to staart in the array.
 * @param max_ptr Maximum of the ptr.
 * @return size_t
 */
size_t char_array_to_tlv(char* s,
                         tlv_t** addr,
                         unsigned long* ptr,
                         size_t max_ptr);

/**
 * @brief Adds a TLV to the message body.
 *
 * @param m Message.
 * @param t TLV.
 * @return short 1 if added else 0.
 */
short msg_add_tlv(msg_t* m, tlv_t* t);

/**
 * @brief Converts a section of a string into a tlv member.
 *
 * @param s Data string.
 * @param ptr Pointer to the start of the member in the string s.
 * @param member Member to initialize.
 * @param size Size of the member.
 * @return size_t
 */
size_t char_to_member(char* s, unsigned long* ptr, void* member, size_t size);

/**
 * @brief Converts a tlv member into a segment of a string.
 *
 * @param s Adress of the string to manipulate.
 * @param ptr Pointer.
 * @param member Member to transform.
 * @param size Size of the member.
 * @return size_t
 */
size_t member_to_char(char** s, unsigned long* ptr, void* member, size_t size);

/**
 * @brief Generates an ACK TLV from the sender ID and Nonce of a data_tlv.
 *
 * @param data_tlv DATA TLV.
 * @param ack_tlv Adress to the new ACK TLV.
 * @return short*
 */
short* ack_from_data(tlv_t* data_tlv, tlv_t** ack_tlv);

/**
 * @brief Generates a nonce from a user ID, using time.
 *
 * @param id
 * @return uint32_t
 */
uint32_t gen_nonce(uint64_t id);

#endif  // MSG_H
