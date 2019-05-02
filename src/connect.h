#ifndef UDP_H_
#define UDP_H_

#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>
#include <netinet/udp.h>

#include <pthread.h>

#include "types.h"

#define ID_BITS_LEN 64
#define REQ_LEN 4096

unsigned char *random_bits(size_t len);
unsigned char *random_id();

// Ça deviendra sans doute udp_server..
void *start_server();


void* udp_server();
void udp_send(char* ip, uint16_t port, sbuff_t* buffer);

#endif  // ! UDP_H_
