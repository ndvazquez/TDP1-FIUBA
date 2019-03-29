#define _GNU_SOURCE
#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "common.h"

#define BUFFER_SIZE 512
#define ARGV_SIZE 3
#define REQUEST_FILE 4

int main(int argc, char *argv[]){
	if (argc < ARGV_SIZE){
		printf("Uso:\n./client <direccion> <puerto> [<input>]\n");
		return 1;
	}

	FILE *request;

	char *request_buffer;
	char *response_buffer;
	ssize_t nread;
	char *line = NULL;
	size_t len;
	size_t request_len;
	struct addrinfo hints;
	struct addrinfo *res;
	int s, skt = 0;
	int ok;

	if (argc == REQUEST_FILE){
		request = fopen(argv[3], "r");
	} else{
		request = stdin;
	}
	request_buffer = malloc(sizeof(char) * BUFFER_SIZE);

	size_t total_read = 0;
	while (((nread = getline(&line, &len, request)) > 0)){
			memcpy(request_buffer + total_read, line, nread);
			total_read += nread;
	}
	
	request_buffer[total_read] = '\0';
	request_len = strlen(request_buffer);

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;       
	hints.ai_socktype = SOCK_STREAM; 
	hints.ai_flags = 0; 

	s = getaddrinfo(argv[1], argv[2], &hints, &res);
	if (s != 0) { 
		printf("Error in getaddrinfo: %s\n", gai_strerror(s));
		fclose(request);
		freeaddrinfo(res);
		free(request_buffer);
		return 1;
	}

	skt = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (skt == -1) {
		printf("Error: %s\n", strerror(errno));
		fclose(request);
		freeaddrinfo(res);
		free(request_buffer);
		return 1;
	}

	s = connect(skt, res->ai_addr, res->ai_addrlen);
	if (s == -1) {
		printf("Error: %s\n", strerror(errno));
		close(skt);
		fclose(request);
		freeaddrinfo(res);
		free(request_buffer);
		return 1;
	}
	freeaddrinfo(res);

	ok = send_msg(skt, request_buffer, request_len);
	shutdown(skt, SHUT_WR);
	if (!ok){
		printf("Couldn't send the message\n");
		fclose(request);
		free(request_buffer);
		shutdown(skt, SHUT_RD);
		close(skt);
		return 1;
	}

	response_buffer = malloc(sizeof(char) * BUFFER_SIZE);
	ok = receive_msg(skt, response_buffer, BUFFER_SIZE);
	shutdown(skt, SHUT_RD);
	if (!ok){
		printf("Couldn't receive a response from the server\n");
		fclose(request);
		free(request_buffer);
		close(skt);
		return 1;
	}

	printf("%s", response_buffer);
	free(line);
	free(request_buffer);
	free(response_buffer);
	close(skt);
	fclose(request);

	return 0;
}
