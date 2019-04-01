#define _GNU_SOURCE
#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "common_socket.h"

#define BUFFER_SIZE 512
#define ARGV_SIZE 3
#define REQUEST_FILE 4

int main(int argc, char *argv[]){
	if (argc < ARGV_SIZE){
		printf("Uso:\n./client <direccion> <puerto> [<input>]\n");
		return 1;
	}

	FILE *request;
	char *host = argv[1];
	char *port = argv[2];
	char *request_buffer;
	char *response_buffer;
	ssize_t nread;
	char *line = NULL;
	size_t len;
	size_t request_len;

	socket_t skt;

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

	if (socket_init(&skt, host, port, 0)){
		printf("Error: %s\n", strerror(errno));
		fclose(request);
		free(request_buffer);
        return 1;
	}

	ok = socket_send_msg(&skt, request_buffer, request_len);

	if (!ok){
		printf("Couldn't send the message\n");
		fclose(request);
		free(request_buffer);
		socket_destroy(&skt);
		return 1;
	}

	response_buffer = malloc(sizeof(char) * BUFFER_SIZE);
	ok = socket_receive_msg(&skt, response_buffer, BUFFER_SIZE);

	if (!ok){
		printf("Couldn't receive a response from the server\n");
		fclose(request);
		free(request_buffer);
		socket_destroy(&skt);
		return 1;
	}
	
	printf("%s", response_buffer);
	free(line);
	free(request_buffer);
	free(response_buffer);
	socket_destroy(&skt);
	fclose(request);

	return 0;
}
