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

#define REQ_LEN 4096


void* udp_server();
void udp_send(char* ip, uint16_t port, sbuff_t* buffer);

#endif  // ! UDP_H_
