// #include "srv.h"

// int srv_init() {
// 	int s;
// 	struct sockaddr_in6 server;
// 	int rc;
// 	unsigned char req[4096];
// 	unsigned char rep[4096];
// 	struct sockaddr_in6 client;
// 	int client_len;

// 	s = socket(AF_INET6, SOCK_DGRAM, 0);

// 	memset(&server, 0, sizeof(server));
// 	server.sin6_family = AF_INET6;
// 	server.sin6_port = htons(PORT);


// 	rc = bind(s, (struct sockaddr *)&server, sizeof(server));
// 	if (rc < 0) {
// 		// euh..
// 	}
// 	while (1) {
// 		unsigned int client_len;
// 		rc = recvfrom(s, req, 4096, 0, (struct sockaddr *)&client, &client_len);
// 		if (rc < 0) {
// 			// hum..
// 			perror("rcvfrom");
// 			continue;
// 		}
// 		// on traite la requête.
// 		char *reply = "hello";
// 		rc = sendto(s, reply, sizeof(reply), 0, (const struct sockaddr *)&client, client_len);
// 	}
// }

// int main(void) {
// 	srv_init();
// 	return 0;
// }