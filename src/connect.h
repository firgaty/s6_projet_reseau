#ifndef UDP_H_
#define UDP_H_

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/udp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>

#include <pthread.h>

#include "msg.h"
#include "types.h"
#include "serialization.h"
#include "shared_resources.h"

#define ID_BITS_LEN 64
#define REQ_LEN 4096
#define DEFAULT_PORT 8090

int sock;

unsigned char* random_bits(size_t len);
unsigned char* random_id();

uint16_t server_port;
void set_server_port(uint16_t port);

// // Ça deviendra sans doute udp_server..
// void* start_server(int loglevel);

void* udp_server();
int udp_send(struct sockaddr_in6* client, sbuff_t* buffer);

void* process_datagram(unsigned char* input, size_t len, struct sockaddr_in6* client);
void process_hello(hello_body_t* b, struct sockaddr_in6* client);
void process_neighbour(neighbour_body_t* b);
void process_data(data_body_t* b, struct sockaddr_in6* client);
void process_warning(warning_body_t* b);
void process_ack(ack_body_t* b);
void process_go_away(go_away_body_t* b);

#endif  // ! UDP_H_
