#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <poll.h>
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
		if(sockfd == -1) {
			fprintf(stderr, "cannot create socket: %s\n", strerror(errno));
			continue;
		}
		if(canbind = (bind(sockfd, rp->ai_addr, rp->ai_addrlen) != -1)) break;
		fprintf(stderr, "cannot bind: %s\n", strerror(errno));

		close(sockfd);
	}
	if(!canbind) exit(EXIT_FAILURE);
	return sockfd;
}

#define BUF_SIZE 256

int main(void) {
	struct addrinfo *res = init_addrinfo();
	int sockfd = bind_addrinfo(res);
	freeaddrinfo(res);
	listen(sockfd, 1);
	for(int fd = -1; (fd = accept(sockfd, NULL, NULL)) != -1;) {
		printf("accept fd = %d\n", fd);
		char buf[BUF_SIZE] = {0};
		for(int nread = 0; nread = recv(fd, buf, sizeof(buf), 0); ) {
			if(nread == -1) {
				fprintf(stderr, "cannot recv: %s", strerror(errno));
				break;
			}
			printf("nread = %d>>>%s<<<", nread, buf);
			fflush(stdout);
		}
		close(fd);
	}
	fprintf(stderr, "accept returns -1: %s\n", strerror(errno));
	close(sockfd);
}
