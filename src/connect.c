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

#include "connect.h"

// unsigned char* random_bits(size_t len) {
//   unsigned char* s;

//   sock = malloc(len);
//   while (len >= 1) {
//     s[len - 1] = rand();
//     len--;
//   }
//   return (s);
// }

// unsigned char* random_id() {
//   return (random_bits(ID_BITS_LEN));
// }

// void* start_server(int loglevel) {
//   unsigned char* id;

//   id = random_id();
//   if (loglevel > 0) {
//     // TODO
//     // Ça c’est une connerie ; d’ailleurs la « chaîne » générée n’est même
//     pas
//     // terminée par \0.
//     printf("ID: %8s\n", id);
//   }

//   int sock = socket(AF_INET, SOCK_DGRAM, 0);
//   if (s < 0) {
//     printf("error créa socket, man\n");
//   }

//   unsigned char req[REQ_LEN];
//   memset(&req, 0, sizeof(req));
//   req[0] = 93;
//   req[1] = 2;
//   req[3] = 0;

//   struct sockaddr_in server;
//   memset(&server, 0, sizeof(server));
//   server.sin_family = AF_INET;
//   server.sin_addr.s_addr = htonl(INADDR_ANY);
//   server.sin_port = htons(DEFAULT_PORT);
//   // char *p = inet_ntop(AF_INET, "81.194.27.155", &server.sin_addr,
//   // sizeof(server.sin_addr));
//   const char* p = inet_ntop(AF_INET, "127.0.0.1", (void*)&server.sin_addr,
//                             sizeof(server.sin_addr));
//   // printf("%c\n", p);

//   int rc = sendto(s, "test", 4, 0, (struct sockaddr*)&server,
//   sizeof(server)); if (rc < 0) {
//     printf("ça a merdé au `sendto`, déso..\n");
//   }

//   printf("Nous nous trouvons après le sendto [!]\n");

//   // unsigned char *reply[REQ_LEN];
//   // rc = recvfrom(s, reply, REQ_LEN, 0, NULL, NULL);
//   // if (rc < 0) {
//   // 	printf("ça a merdé au `recvfrom`, déso..\n");
//   // }
//   close(s);
// }

void* udp_server() {
  printf("Server start.\n");
  sock = socket(AF_INET6, SOCK_DGRAM, 0);

  if (sock < 0) {
    perror("Socket.");
    exit(-1);
  }

  ssize_t rc = fcntl(sock, F_GETFL);
  if (rc < 0) {
    perror("F_GETFL.");
    exit(-1);
  }
  if (fcntl(sock, F_SETFL, rc | O_NONBLOCK) < 0) {
    perror("SETFL.");
    exit(-1);
  }

  int to = 10;

  struct sockaddr_in6 server;
  memset(&server, 0, sizeof(server));
  server.sin6_family = AF_INET6;
  server.sin6_port = htons(server_port);

  if (bind(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
    perror("Erreur bind.\n");
    return 0;
  }

  while (1) {
    printf("Loop\n");
    unsigned char req[REQ_LEN];
    struct sockaddr_in6 client;
    int client_len = sizeof(client);

    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(sock, &readfds);
    struct timeval tv = {to, 0};
    rc = select(sock + 1, &readfds, NULL, NULL, &tv);
    if (rc < 0) {
      perror("select.");
      continue;
    } else if (rc == 0) {
      continue;
    }

    ssize_t rc = recvfrom(sock, req, REQ_LEN, 0, (struct sockaddr*)&client,
                          (socklen_t*)&client_len);
    if (rc < 0) {
      printf("Erreur recvfrom\n");
      continue;
    }
    printf("Message received.\n");

    // char *client_ip = malloc(sizeof(char) * 16);
    // inet_ntop(AF_INET6, &(client.sin6_addr), client_ip, INET6_ADDRSTRLEN);

    process_datagram(req, rc, &client);
  }

  close(sock);
  return 0;
}

void udp_send(unsigned char* ip, uint16_t port, sbuff_t* buffer) {
  if (sock < 0) {
    // gerer erreur.
    printf("Erreur init envoit\n");
    return;
  }
  // unsigned char req[REQ_LEN];

  struct sockaddr_in6 pair;
  // remplir la sockaddr...
  memset(&pair, 0, sizeof(pair));
  pair.sin6_family = AF_INET6;
  pair.sin6_port = htons(port);
  memcpy(&pair.sin6_addr, ip, 16);

  // if (bind(s, (struct sockaddr*)&pair, sizeof(struct sockaddr_in6)) < 0) {
  //   // gerer erreur.
  //   printf("Erreur bind envoit\n");
  //   return;
  // }
  char str_ip[INET6_ADDRSTRLEN];
  inet_ntop(AF_INET6, &pair.sin6_addr, str_ip, INET6_ADDRSTRLEN);

  printf("IP: %s, Port: %u\n", str_ip, ntohs(pair.sin6_port));

  ssize_t rc = sendto(sock, buffer->data, buffer->real_size, 0,
                      (struct sockaddr*)&pair, sizeof(pair));
  if (rc < 0) {
    perror("Error: sendto... \n");
    return;
  }

  printf("ENVOIT SUCCESS !!!\n");
}

void set_server_port(uint16_t port) {
  server_port = port <= 0 ? DEFAULT_PORT : port;
}

void* process_datagram(unsigned char* input,
                       size_t len,
                       struct sockaddr_in6* client) {
  // char str_ip[INET6_ADDRSTRLEN];
  //   inet_ntop(AF_INET6, &client->sin6_addr, str_ip, INET6_ADDRSTRLEN);
  //   printf("IP: %s, Port: %u\n", str_ip, ntohs(client->sin6_port));

  sbuff_t* buffer = new_dsbuff(input, len);
  msg_t* msg = dserial_msg(buffer);

  if (msg->magic != 93 || msg->version != 2)
    return 0;

  // TODO : faire un vrai traitement...
  for (int i = 0; i < msg->tlv_nb; i++) {
    switch (msg->body[i]->type) {
      case TLV_PAD1:
        return 0;
        break;
      case TLV_PADN:
        return 0;
        break;
      case TLV_HELLO:
        process_hello((hello_body_t*)msg->body[i]->body, client);
        break;
      case TLV_NEIGHBOUR:
        break;
      case TLV_DATA:
        break;
      case TLV_ACK:
        break;
      case TLV_GO_AWAY:
        break;
      case TLV_WARNING:
        break;
    }
  }
  return 0;
}

void process_hello(hello_body_t* b, struct sockaddr_in6* client) {
  char str_ip[INET6_ADDRSTRLEN];
  inet_ntop(AF_INET6, &client->sin6_addr, str_ip, INET6_ADDRSTRLEN);
  printf("HELLO: %lu, ", b->source_id);
  if (b->is_long)
    printf("%lu", b->dest_id);
  printf("| %.*s : ", INET6_ADDRSTRLEN, str_ip);
  printf("%u\n", ntohs(client->sin6_port));
}

void process_neighbour(neighbour_body_t* b);
void process_data(data_body_t* b);
void process_warning(warning_body_t* b);
void process_ack(ack_body_t* b);
void process_go_away(go_away_body_t* b);

void add_nbr(unsigned char* host, char* port) {
  printf("add_nbr\n");
  struct addrinfo hint = {0};
  hint.ai_family = PF_INET6;
  hint.ai_flags = AI_V4MAPPED | AI_ALL;
  hint.ai_socktype = SOCK_DGRAM;

  struct addrinfo* ai;

  // printf("before getaddrinfo\n");
  int rc = getaddrinfo((char*)host, port, &hint, &ai);
  if (rc != 0) {
    printf("%s\n", gai_strerror(rc));
    return;
  }

  // printf("before sb\n");

  struct addrinfo* p = ai;
  tlv_t* t = new_tlv_hello(false, client_id, 0);
  msg_t* m = new_msg(&t, 1);
  sbuff_t* sb = new_sbuff();
  serial_msg(m, sb);

  // printf("before loop\n");
  // char ip_str[INET6_ADDRSTRLEN];

  for (; p; p = p->ai_next) {
    // inet_ntop(AF_INET6, p->ai_addr, ip_str, p->ai_addrlen);
    // printf("%s\n", ip_str);
    udp_send(((struct sockaddr_in6*)p->ai_addr)->sin6_addr.__in6_u.__u6_addr8,
             ntohs(((struct sockaddr_in6*)p->ai_addr)->sin6_port), sb);
    printf("post_send\n");
  }
}
