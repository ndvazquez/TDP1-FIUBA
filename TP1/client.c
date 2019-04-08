#define _GNU_SOURCE
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
	if (argc == REQUEST_FILE){
		request = fopen(argv[3], "r");
	} else{
		request = stdin;
	}

	fseek(request, 0, SEEK_END);
	size_t request_len = ftell(request);
	fseek(request, 0, SEEK_SET);

	char *request_buffer;
	request_buffer = malloc(sizeof(char) * request_len + 1);
	size_t total_read = fread(request_buffer, 1, request_len, request);
	if (total_read != request_len){
		free(request_buffer);
		fclose(request);
	}
	request_buffer[request_len] = '\0';

	char *host = argv[1];
	char *port = argv[2];
	socket_t skt;
	if (socket_init(&skt, host, port, 0)){
		printf("Error: %s\n", strerror(errno));
		fclose(request);
		free(request_buffer);
        return 1;
	}

	int ok;
	ok = socket_send_msg(&skt, request_buffer, request_len);

	if (!ok){
		printf("Couldn't send the message\n");
		fclose(request);
		free(request_buffer);
		socket_destroy(&skt);
		return 1;
	}

	char *response_buffer;
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
	free(request_buffer);
	free(response_buffer);
	socket_destroy(&skt);
	fclose(request);

	return 0;
}
