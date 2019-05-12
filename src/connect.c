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
#include "neighbour_map.h"

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
    exit(-1);
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
        break;
      case TLV_PADN:
        return 0;
        break;
      case TLV_HELLO:
        process_hello((hello_body_t*)msg->body[i]->body, client);
        break;
      case TLV_NEIGHBOUR:
        process_neighbour((neighbour_body_t*)msg->body[i]->body);
        break;
      case TLV_DATA:
        process_data((data_body_t*)msg->body[i]->body, client);
        break;
      case TLV_ACK:
        process_ack((ack_body_t*)msg->body[i]->body, client);
        break;
      case TLV_GO_AWAY:
        process_go_away((go_away_body_t*)msg->body[i]->body, client);
        break;
      case TLV_WARNING:
        process_warning((warning_body_t*)msg->body[i]->body);
        break;
    }
  }

  free_msg(msg, false);
  free_sbuff(buffer);
  return 0;
}

void send_msg(struct sockaddr_in6* pair, msg_t* msg) {
  sbuff_t* buff = new_sbuff();
  serial_msg(msg, buff);
  udp_send(pair, buff);
  free_sbuff(buff);
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
  free_msg(m, true);
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

  free(t);
  free(m);
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

  // ajout si pas dans les neighbour.
  neighbour_map_t* cur = get_cur_neighbours();
  neighbour_map_t* pot = get_pot_neighbours();

  char* key = new_neighbour_key_sock(client);
  neighbour_entry_t* e_cur = *map_get(cur, key);
  neighbour_entry_t* e_pot = *map_get(pot, key);

  if (e_cur != NULL) {
  } else if (e_cur == NULL && e_pot == NULL) {
    char str_ip[INET6_ADDRSTRLEN];
    inet_ntop(AF_INET6, &client, str_ip, INET6_ADDRSTRLEN);
    char str_port[10];
    snprintf(str_port, 10, "%u", client->sin6_port);
    add_new_neighbour((unsigned char*)str_ip, (char*)str_port);
  } else if (e_pot != NULL) {
    map_transfer_neighbour(pot, cur, key);
  }

  free(key);

  pthread_mutex_unlock(&cur_neighbours_lock);
  pthread_mutex_unlock(&pot_neighbours_lock);

  free_hello_body(b);
}

void process_neighbour(neighbour_body_t* b) {
  char str_ip[INET6_ADDRSTRLEN];
  inet_ntop(AF_INET6, &b->ip, str_ip, INET6_ADDRSTRLEN);
  char str_port[10];
  snprintf(str_port, 10, "%u", b->port);

  add_new_neighbour((unsigned char*)str_ip, (char*)str_port);
  free_neighbour_body(b);
}

void process_data(data_body_t* b, struct sockaddr_in6* client) {
  printf("DATA\n");
  // envoit du TLV ACK.
  send_ack(client, b->sender_id, b->nonce);

  // Traitement.
  add_msg(b);

  // Si d'un type non implementé, on ne fait rien.
  if (b->type != 0)
    return;

  // fonction qui affiche le contenu d'un DATA.
  printf("%.*s", (int)b->data_len, b->data);
}
void process_warning(warning_body_t* b) {
  // print warning
  printf("/!\\ %.*s\n", (int)b->msg_len, b->message);
  free_warning_body(b);
}

void process_ack(ack_body_t* b, struct sockaddr_in6* pair) {
  char* key = new_neighbour_key_sock(pair);
  neighbour_map_t* cur = get_cur_neighbours();

  neighbour_entry_t* e = *map_get(cur, key);
  if (e != NULL) {
    rm_nbr_msg(b->sender_id, b->nonce, e->msg_to_send);
  }
  pthread_mutex_unlock(&cur_neighbours_lock);

  free_ack_body(b);
}

void process_go_away(go_away_body_t* b, struct sockaddr_in6* pair) {
  neighbour_map_t* cur = get_cur_neighbours();
  neighbour_map_t* pot = get_pot_neighbours();

  char* key = new_neighbour_key_sock(pair);

  neighbour_entry_t* e = *map_get(cur, key);
  dllist_empty(e->msg_to_send, false);

  map_transfer_neighbour(cur, pot, key);
  free(key);

  pthread_mutex_unlock(&cur_neighbours_lock);
  pthread_mutex_unlock(&pot_neighbours_lock);

  free_go_away_body(b);
}
