#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>

//Given a binary file and a float pointer, the function
//reads two bytes from the file and stores them in the
//float pointed to by the pointer.
//Returns 0 if it couldn't read from the file, else elements read.
size_t _read_sensor(FILE *sensor, float *temperature);

//Given a HTTP request, the function checks if the request is valid.
//If the header contains GET /sensor, it returns status code 200.
//If the action is not GET, it returns status code 400.
//If the URI is not /sensor, it returns status code 404.
const char* _parse_request(char *request, char **ptr);

//Given a HTML template, a temperature and a HTTP status code,
//this function prepares the response sent to the client.
void _prepare_body(FILE *template, float temperature, char *response);

//Given a socket, a HTML template, a temperature and a Browser Counter,
//this function accepts clients and handles the connection.
//Returns 0 if the client was served properly, or 1 if something failed.
int _accept_client(int skt, FILE *template, float temperature,
                                        browser_counter_t * bc);

#endif
