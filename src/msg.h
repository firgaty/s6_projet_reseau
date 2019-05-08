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

/**
 * @file msg.h
 * @author Félix Desmaretz 
 * @brief All functions relative to tlv_t and msg_t manipulations.
 * @version 0.1
 * @date 2019-05-04
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include "types.h"

/**
 * @brief Prints the TLV displaying all the useful informations.
 *
 * @param t TLV to print.
 */
void print_tlv(tlv_t* t);

/**
 * @brief Prints the message displaying all the useful informations.
 *
 * @param m Message to print.
 */
void print_msg(msg_t* m);

void print_padn(padn_body_t* b);
void print_hello(hello_body_t* b);
void print_neighbour_body(neighbour_body_t* b);
void print_data(data_body_t* b);
void print_ack(ack_body_t* b);
void print_go_away(go_away_body_t* b);
void print_warning(warning_body_t* b);

#endif  // MSG_H
