#include "connect.h"

unsigned char *random_bits(size_t len) {
	unsigned char *s;

	s = malloc(len);
	while (len >= 1) {
		s[len - 1] = rand();
		len--;
	}
	return (s);
}

unsigned char *random_id() {
	return (random_bits(ID_BITS_LEN));
}

void *start_server() {
	unsigned char *id = random_id();
}

void* udp_server() {}

void udp_send(char* ip, uint16_t port, sbuff_t* buffer) {
  int s = socket(AF_INET6, SOCK_DGRAM, 0);
  if (s < 0) {
    // gerer erreur.
    printf("erreur init envoit");
    return;
  }
  unsigned char req[REQ_LEN];

  // struct sockaddr_sin6 pair;
  // // remplir la sockaddr...

  // if (bind(s, (struct sockaddr*)&pair, sizeof(struct sockaddr_sin6)) < 0) {
  //   // gerer erreur.
  //   printf("erreur bind envoit");
  //   return;
  // }

  // ssize_t rc = sendto(s, req, buffer->data, buffer->real_size,
  //                     (struct sockaddr*)&pair, sizeof(struct sockaddr_sin6));
}