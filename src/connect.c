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

char *host2ip(const char *hostname) {
	int rc;
	struct addrinfo hints;
	struct addrinfo *res, *p;
	int s;
	char *ip;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET6;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = 0;
	hints.ai_flags |= AI_V4MAPPED | AI_ALL;

	rc = getaddrinfo(hostname, NULL, &hints, &res);
	DEBUG_MSG("host2ip: après getaddrinfo")
	if (rc < 0) {
		fprintf(stderr, "[!] getaddrinfo: %s\n", gai_strerror(rc));
		exit(1);
	}
	for(p = res ; p != NULL ; p = p->ai_next) {
		s = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (s < 0) {
			close(s);
			continue;
		}
		if (rc >= 0) {
			break;
		}
		close(s);
	}
	DEBUG_MSG("host2ip: après la boucle for")
	if (p == NULL) {
		fprintf(stderr, "could not reach hostname %s\n", hostname);
		exit(1);
	}
	ip = calloc(INET6_ADDRSTRLEN + 1, sizeof(char));
	inet_ntop(p->ai_family, &((struct sockaddr_in6 *)p->ai_addr)->sin6_addr, (void *)ip, INET6_ADDRSTRLEN);
	freeaddrinfo(res);
	return (ip);
}

void *start_server(int loglevel) {
	unsigned char *id;

	id = random_id();
	if (loglevel > 0) {
		// TODO
		// Ça c’est une connerie ; d’ailleurs la « chaîne » générée n’est même pas terminée par \0.
		printf("ID: %8s\n", id);
	}

	int s = socket(AF_INET, SOCK_DGRAM, 0);
	if (s < 0) {
		printf("error créa socket, man\n");
	}

	unsigned char req[REQ_LEN];
	memset(&req, 0, sizeof(req));
	req[0] = 93;
	req[1] = 2;
	req[3] = 0;

	struct sockaddr_in server;
	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(DEFAULT_PORT);
	// char *p = inet_ntop(AF_INET, "81.194.27.155", &server.sin_addr, sizeof(server.sin_addr));
	const char *p = inet_ntop(AF_INET, "127.0.0.1", (void *)&server.sin_addr, sizeof(server.sin_addr));
	// printf("%c\n", p);

	int rc = sendto(s, "test", 4, 0, (struct sockaddr *)&server, sizeof(server));
	if (rc < 0) {
		printf("ça a merdé au `sendto`, déso..\n");
	}

	printf("Nous nous trouvons après le sendto [!]\n");

	// unsigned char *reply[REQ_LEN];
	// rc = recvfrom(s, reply, REQ_LEN, 0, NULL, NULL);
	// if (rc < 0) {
	// 	printf("ça a merdé au `recvfrom`, déso..\n");
	// }
	close(s);
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
  // memset(&pair, 0, sizeof(pair));
  // pair.sin6_family = AF_INET6;
  // pair.sin6_port = htons(DEFAULT_PORT);

  // if (bind(s, (struct sockaddr*)&pair, sizeof(struct sockaddr_sin6)) < 0) {
  //   // gerer erreur.
  //   printf("erreur bind envoit");
  //   return;
  // }

  // ssize_t rc = sendto(s, req, buffer->data, buffer->real_size,
  //                     (struct sockaddr*)&pair, sizeof(struct sockaddr_sin6));
}