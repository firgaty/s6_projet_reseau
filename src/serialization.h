#ifndef SERIALIZATION_H_
#define SERIALIZATION_H_

#include <arpa/inet.h>
#include <byteswap.h>
#include <inttypes.h>
#include <netinet/in.h>
#include <sys/types.h>

#include "types.h"

void serial_char(char c, sbuff_t* b);
void serial_short(uint16_t i, sbuff_t* b);
void serial_int(int i, sbuff_t* b);
void serial_long(uint64_t l, sbuff_t* b);
void serial_str(char* s, size_t str_len, sbuff_t* b);

void serial_msg(msg_t* m, sbuff_t* b);
void serial_tlv(tlv_t* t, sbuff_t* b);

void serial_padn_body(padn_body_t* e, sbuff_t* b);
void serial_hello_body(hello_body_t* e, sbuff_t* b);
void serial_neighbour_body(neighbour_body_t* e, sbuff_t* b);
void serial_data_body(data_body_t* e, sbuff_t* b);
void serial_ack_body(ack_body_t* e, sbuff_t* b);
void serial_go_away_body(go_away_body_t* e, sbuff_t* b);
void serial_warning_body(warning_body_t* e, sbuff_t* b);

//###############################################

char dserial_char(sbuff_t* b);
uint16_t dserial_short(sbuff_t* b);
uint32_t dserial_int(sbuff_t* b);
uint64_t dserial_long(sbuff_t* b);
char* dserial_str(size_t str_len, sbuff_t* b);

msg_t* dserial_msg(sbuff_t* b);
tlv_t* dserial_tlv(sbuff_t* b);

padn_body_t* dserial_padn_body(size_t tlv_len, sbuff_t* b);
hello_body_t* dserial_hello_body(size_t tlv_len, sbuff_t* b);
neighbour_body_t* dserial_neighbour_body(size_t tlv_len, sbuff_t* b);
data_body_t* dserial_data_body(size_t tlv_len, sbuff_t* b);
ack_body_t* dserial_ack_body(size_t tlv_len, sbuff_t* b);
go_away_body_t* dserial_go_away_body(size_t tlv_len, sbuff_t* b);
warning_body_t* dserial_warning_body(size_t tlv_len, sbuff_t* b);


#endif  // !SERIALIZATION_H_