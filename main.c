#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

struct addrinfo *init_addrinfo(void) {
	struct addrinfo hints;
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_protocol = 0;
	hints.ai_canonname = NULL;
	hints.ai_addr = NULL;
	hints.ai_next = NULL;

	struct addrinfo *ret = NULL;
	int ec = getaddrinfo(NULL, "8080", &hints, &ret);
	if(ec) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(ec));
		exit(EXIT_FAILURE);
	}
	return ret;
}

int bind_addrinfo(struct addrinfo *addrlist) {
	bool canbind = false;
	int sockfd = 0;
	for(struct addrinfo *rp = addrlist; rp != NULL; rp = rp->ai_next) {
		sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if(sockfd == -1) continue;
		if(canbind = !bind(sockfd, rp->ai_addr, rp->ai_addrlen)) break;

		close(sockfd);
	}
	if(!canbind) {
		fprintf(stderr, "cannot bind\n");
		exit(EXIT_FAILURE);
	}
	return sockfd;
}

int main(void) {
	struct addrinfo *res = init_addrinfo();
	int sockfd = bind_addrinfo(res);
	freeaddrinfo(res);
	listen(sockfd, 1);
	sleep(60);
}
