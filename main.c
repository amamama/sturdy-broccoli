#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <assert.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <poll.h>
#include <netdb.h>
#include <unistd.h>

#include <jansson.h>

#define debug(...) (fprintf(stderr, "%s:%d:%s:", __FILE__, __LINE__, __func__), fprintf(stderr,  __VA_ARGS__))
#define error(errno, ...) (debug("%s:", strerror(errno)), fprintf(stderr, __VA_ARGS__))

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
	if(ec) debug("getaddrinfo: %s\n", gai_strerror(ec)), exit(EXIT_FAILURE);
	return ret;
}

int bind_addrinfo(struct addrinfo *addrlist) {
	bool canbind = false;
	int sockfd = 0;
	for(struct addrinfo *rp = addrlist; rp != NULL; rp = rp->ai_next) {
		sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if(sockfd == -1) {
			error(errno, "cannot create socket\n");
			continue;
		}
		if(canbind = (bind(sockfd, rp->ai_addr, rp->ai_addrlen) != -1)) break;
		error(errno, "cannot bind\n");
		close(sockfd);
	}
	if(!canbind) exit(EXIT_FAILURE);
	return sockfd;
}

#define BUF_SIZE 256

typedef struct most_really_awesome_cool_convenient_string_type_in_the_universe {
	size_t length;
	size_t size;
	char *str;
} string;

size_t ceiling_pow2(size_t n) {
	if(n == 0 || n > SIZE_MAX / 2) debug("%zd is too big to ceil\n", n);
	n--;
	for(size_t i = 1, nbits = sizeof(SIZE_MAX) * CHAR_BIT; i < nbits; i <<= 1)
		n |= n >> i;
	n++;
	return n;
}

string alloc_string(char *str) {
	string ret = {0, 0, NULL};
	size_t len = strlen(str);
	if(!len) {
		ret.str = malloc(sizeof(char[0 + 1])); //for +1 null character
		ret.str[0] = 0;
		return ret;
	}

	ret.length = len;
	ret.size = ceiling_pow2(len + 1); //+1 for null character
	if(ret.size < BUF_SIZE) ret.size = BUF_SIZE;
	ret.str = malloc(sizeof(char[ret.size]));
	strncpy(ret.str, str, len + 1); //+1 for null character
	return ret;
}

string free_string(string str) {
	free(str.str);
	str.str = NULL;
	str.length = 0;
	str.size = 0;
	return str;
}
#define free_string(str) (str = free_string(str))

string concat_string_rawstr(string dest, char *str) {
	assert(dest.str != str);
	assert(dest.str != NULL);
	size_t len = strlen(str);
	if(dest.size < dest.length + len + 1) { //+1 for null character
		dest.size = ceiling_pow2(dest.length + len + 1);
		dest.str = realloc(dest.str, sizeof(char[dest.size]));
	}
	dest.length += len;
	debug("dest.size = %zu, dest.length = %zu, len = %zu\n", dest.size, dest.length, len);
	strncat(dest.str, str, len + 1); //+1 for null character
	return dest;
}

string concat_string(string dest, string src) {
	assert(src.str != NULL);
	return concat_string_rawstr(dest, src.str);
}

int recv_string(int fd, string *out) {
	int len = 0;
	for(int nrecv = BUF_SIZE; nrecv == BUF_SIZE; ) {
		char buf[BUF_SIZE] = {0};
		len += nrecv = recv(fd, buf, sizeof(buf) - 1, 0); //-1 for null character
		if(nrecv == -1) { error(errno, "cannot recv\n"); break; }
		debug("nrecv = %d\n", nrecv);
		*out = concat_string_rawstr(*out, buf);
	}
	return len;
}

int send_string(int fd, string str) {
	return send(fd, str.str, str.length, 0);
}

typedef struct {
	enum {
		GET,
		POST,
	} method;
	int id;
	string body;
} api_t;

api_t parse_HTTP_request(string str) { //caller needs to free ret.str
	char method_name[8] = "";
	int id = -1, sscanf_num = 0;

	char *tok = strtok(str.str, "\r");
	//strtok treats second argument as a set of delimiters so this arguments are not interpreted "\r\n" but '\r' or '\n'.
	//and strtok always returns nonempty string(see man strtok), strtok("aaa\r\nbbb", "\r\n") returns "aaa" and "bbb".
	//but HTTP body is following "\r\n\r\n", string needs to be devided by "\r".
	sscanf_num = sscanf(tok, "%s /api/v1/event%*c%d HTTP/1.1", method_name, &id);
	debug("sscanf_num = %d, %s, %d\n", sscanf_num, method_name, id);

	for(;tok = strtok(NULL, "\r");) {
		debug("header = [%s]\n", tok);
		if(tok[0] == '\n' && tok[1] == '\0') break;
	}
	tok = strtok(NULL, ""); tok++; // ++ for '\n' character
	debug("body = [%s]\n", tok);
	string body = alloc_string(tok);
	tok = strtok(NULL, "");
	assert(tok == NULL);

	api_t ret = {GET, id, body};
	/**/ if(!strncmp("GET", method_name, sizeof(method_name))) ret.method = GET;
	else if(!strncmp("POST", method_name, sizeof(method_name))) ret.method = POST;
	return ret;
}

static json_t *todo = NULL;
int init_todo(void) {
	if(todo) return 0;
	json_error_t err;
	todo = json_loads("{\"events\":[]}", 0, &err);
	return 0;
}

int send_todo(int fd) {
	return json_dumpfd(todo, fd, 0);
}

#define HTTP_STATUS_404 "404 Not Found\r\n"
int get_respond(int id, int fd) {
	if(id < 0) return send_todo(fd);
	size_t len = json_array_size(json_object_get(todo, "events"));
	if(len <= id) return send(fd, HTTP_STATUS_404, strlen(HTTP_STATUS_404), 0);
	return 0;
}

int post_respond(string body, int fd) {
	return 0;
}

int respond(api_t res, int fd) {
	switch(res.method) {
		case GET: get_respond(res.id, fd); break;
		case POST: post_respond(res.body, fd); break;
		default: debug("an error occured. method = %d\n", res.method);
	}
	debug("method = %d, id = %d, body = %s\n", res.method, res.id, res.body.str);
	free_string(res.body);
	return 0;
}

int main(void) {
	init_todo();

	size_t i = 0, nbits = sizeof(SIZE_MAX) * CHAR_BIT;
	struct addrinfo *res = init_addrinfo();
	int sockfd = bind_addrinfo(res); freeaddrinfo(res);
	listen(sockfd, 1);
	for(int fd = -1; (fd = accept(sockfd, NULL, NULL)) != -1;) {
		debug("accept fd = %d\n", fd);
		string str = alloc_string("");
		int nrecv = recv_string(fd, &str);
		debug("%s", str.str);
		send_string(fd, str);
		respond(parse_HTTP_request(str), fd); //parse overwrites str
		close(fd);
	}
	error(errno, "accept returns -1:\n");
	close(sockfd);
}
