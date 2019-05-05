#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define DEFAULT_PORT 8090
#define REQ_LEN 1024

int main(void) {
	int s;
	// struct sockaddr_in6 client;
	struct sockaddr_in6 server;
	unsigned char req[REQ_LEN];
	unsigned char rep[4096];
	// char *req;

	if ((s = socket(AF_INET6, SOCK_DGRAM, 0)) < 0) {
		perror("La socket n’a pas pu être crée");
		exit(EXIT_FAILURE);
	}

	// memset(&client, 0, sizeof(client));
	// client.sin6_family = AF_INET6;

	memset(&server, 0, sizeof(server));
	server.sin6_family = AF_INET6;
	server.sin6_port = htons(DEFAULT_PORT);
	inet_ntop(AF_INET6, "2001:660:3301:9200::51c2:1b9b", (void *)&server.sin6_addr, sizeof(server.sin6_addr));
	// inet_ntop(AF_INET6, "::1", (void *)&server.sin6_addr, sizeof(server.sin6_addr));

	memset(&req, 0, REQ_LEN);
	// req = "test";
	req[0] = 93;
	req[1] = 2;
	req[2] = 0;
	if ((sendto(s, &req, strlen(req), 0, (struct sockaddr *)&server, sizeof(server))) < 0) {
		perror("La requête n’a pas pu être envoyée");
		exit(EXIT_FAILURE);
	}

	printf("[OK] Requête envoyée.\n");

	if ((recvfrom(s, rep, 4096, 0, NULL, NULL)) < 0) {
		perror("Erreur à la réception");
		exit(EXIT_FAILURE);
	}

	close(s);

	// if ((bind(s, (const struct sockaddr *)&server, sizeof(server))) < 0) {
	// 	perror("Le `bind` a échoué");
	// 	exit(EXIT_FAILURE);
	// }

	return (EXIT_SUCCESS);
}