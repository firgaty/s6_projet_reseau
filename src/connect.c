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

int udp_send(struct sockaddr_in6* pair, sbuff_t* buffer) {
  if (sock < 0) {
    // gerer erreur.
    printf("Erreur init envoit\n");
    return -1;
  }
  // unsigned char req[REQ_LEN];

  // if (bind(s, (struct sockaddr*)&pair, sizeof(struct sockaddr_in6)) < 0) {
  //   // gerer erreur.
  //   printf("Erreur bind envoit\n");
  //   return
  // }
  char str_ip[INET6_ADDRSTRLEN];
  inet_ntop(AF_INET6, &pair->sin6_addr, str_ip, INET6_ADDRSTRLEN);

  printf("IP: %s, Port: %u\n", str_ip, ntohs(pair->sin6_port));

  ssize_t rc = sendto(sock, buffer->data, buffer->real_size, 0,
                      (struct sockaddr*)pair, sizeof(*pair));
  if (rc < 0) {
    perror("Error: sendto... \n");
    return -1;
  }

  printf("ENVOIT SUCCESS !!!\n");
  return 1;
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

void send_hello(struct sockaddr_in6* pair,
                bool is_long,
                uint64_t src_id,
                uint64_t dst_id) {
  tlv_t* t = new_tlv_hello(is_long, src_id, dst_id);
  msg_t* m = new_msg(&t, 1);
  sbuff_t* b = new_sbuff();
  serial_msg(m, b);
  udp_send(pair, b);
  free_msg(t, true);
  free_sbuff(b);
}

void send_ack(struct sockaddr_in6* pair, uint64_t sender_id, uint32_t nonce) {
  tlv_t* t = new_tlv_ack(sender_id, nonce);
  msg_t* m = new_msg(&t, 1);

  sbuff_t* buff = new_sbuff();
  serial_msg(m, buff);

  udp_send(pair, buff);

  free_msg(m, true);
  free_sbuff(buff);
}

void send_go_away(struct sockaddr_in6* pair) {
  tlv_t* t = new_tlv_go_away(2, NULL, 0);
  msg_t* m = new_msg(&t, 1);

  sbuff_t* buff = new_sbuff();
  serial_msg(m, buff);

  udp_send(pair, buff);

  free_msg(m, true);
  free_sbuff(buff);
}

void process_hello(hello_body_t* b, struct sockaddr_in6* client) {
  char str_ip[INET6_ADDRSTRLEN];
  inet_ntop(AF_INET6, &client->sin6_addr, str_ip, INET6_ADDRSTRLEN);
  printf("HELLO: %lu, ", b->source_id);
  if (b->is_long)
    printf("%lu", b->dest_id);
  printf("| %.*s : ", INET6_ADDRSTRLEN, str_ip);
  printf("%u\n", ntohs(client->sin6_port));

  if (!b->is_long) {
    send_hello(client, true, b->source_id, client_id);
  }

  free_hello_body(b);
}

void process_neighbour(neighbour_body_t* b);
void process_data(data_body_t* b, struct sockaddr_in6* client) {
  // envoit du TLV ACK.
  send_ack(client, b->sender_id, b->nonce);

  // Traitement.
  dllist_t* l = get_msg_list();

  // TODO

  pthread_mutex_unlock(&msg_list_lock);

  // Si d'un type non implementé, on ne fait rien.
  if (b->type != 0)
    return;

  // fonction qui affiche le contenu d'un DATA.
  printf("%.*s", b->data_len, b->data);
}
void process_warning(warning_body_t* b) {
  printf("/!\\ %.*s\n", b->msg_len, b->message);

  // print warning
}

void process_ack(ack_body_t* b);

void process_go_away(go_away_body_t* b);
