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
#define HEADER_SIZE 50
#define ARGV_SIZE 4
static const char GET[] = "GET ";
static const char USER_AGENT[] = "User-Agent: ";
static const char URI[] = "GET /sensor ";
static const char OK[] = "200 OK";
static const char BAD_REQUEST[] = "400 Bad request";
static const char NOT_FOUND[] = "404 Not found";
static const char TEMPLATE_DATA[] = "{{datos}}";

size_t _read_sensor(FILE *sensor, float *temperature){
    uint16_t ne;
    uint16_t he;
    size_t elements_read = 0;

    if ((elements_read = fread(&ne, sizeof(uint16_t), 1, sensor)) > 0){
        he = ntohs(ne); 
        *temperature = (he - 2000) / 100.0; 
    }
    return elements_read;
}

const char* _parse_request(char *request, char **ptr){
    char *request_action;
    char *request_uri;
    char *request_user_agent;
    size_t browser_lenght = 0;

    request_action = strstr(request, GET);
    if (!request_action || (request - request_action) != 0) return BAD_REQUEST;
    request_uri = strstr(request, URI);
    if (!request_uri || (request - request_action) != 0) return NOT_FOUND;
    request_user_agent = strstr(request, USER_AGENT);
    if (request_user_agent) {
        request_user_agent += strlen(USER_AGENT);
        while (*(request_user_agent+browser_lenght) != '\n') {
            browser_lenght++;
        }
        char *browser = malloc(sizeof(char) * (browser_lenght + 1));
        memcpy(browser, request_user_agent, browser_lenght);
        browser[browser_lenght] = '\0';
        *ptr = browser;
    }
    return OK; 
} 

void _prepare_body(FILE *template, float temperature, char *response){
    char *line = NULL;
    size_t getline_len = 0;
    ssize_t nread;

    size_t total_read = 0;
    size_t first_half;
    size_t rep_len = strlen(TEMPLATE_DATA);
    char *replacement;
    char temp[10];

    snprintf(temp, sizeof(temp), "%.2f", temperature);

    while ((nread = getline(&line, &getline_len, template)) > 0){
        if ((replacement = strstr(line, TEMPLATE_DATA))){
            first_half = replacement - line;
            memcpy(response, line, first_half);
            response += first_half;
            memcpy(response, temp, strlen(temp));
            response += strlen(temp);
            memcpy(response, replacement + rep_len, nread-first_half-rep_len);
            response += nread - first_half - rep_len;
            nread -= rep_len - strlen(temp);
        } else {
             memcpy(response, line, nread);
             response += nread;
        }
        total_read += nread;
    }
    *(response) = '\n';
    *(response+1) = '\0';
    free(line);
    fseek(template, 0, SEEK_SET);
}

int main(int argc, char *argv[]){
    if (argc < ARGV_SIZE){
        printf("Uso:\n./server <puerto> <input> [<template>]\n");
        return 1;
    }
    // TODO: Recibir bien los parametros.
    FILE *sensor;
    FILE *template;

    int ok;
    char *port = argv[1];
    int s = 0;
    int skt, peerskt = 0;
    struct addrinfo hints;
    struct addrinfo *results;

    float temperature;

    char *user_agent = NULL; // Buffer para guardar el browser
    char *request_buffer;
    char *response_buffer;
    char header[BUFFER_SIZE];
    char *ptr_body;

    sensor = fopen(argv[2], "rb");
    if (!sensor){
        printf("Error: %s\n", strerror(errno));
        return 1;
    }

    template = fopen(argv[3], "r");
    if (!template){
        printf("Error: %s\n", strerror(errno));
        return 1;
    }

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    s = getaddrinfo("localhost", port, &hints, &results);
    if (s != 0) { 
      printf("Error in getaddrinfo: %s\n", gai_strerror(s));
      return 1;
    }

    skt = socket(results->ai_family, results->ai_socktype, results->ai_protocol);
    if (skt == -1) {
      printf("Error: %s\n", strerror(errno));
      freeaddrinfo(results);
      return 1;
    }

    int val = 1;
    s = setsockopt(skt, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));
    if (s == -1) {
      printf("Error: %s\n", strerror(errno));
      close(skt);
      freeaddrinfo(results);
      return 1;
    }

    s = bind(skt, results->ai_addr, results->ai_addrlen);
    if (s == -1) {
      printf("Error: %s\n", strerror(errno));
      close(skt);
      freeaddrinfo(results);
      return 1; 
    }

    freeaddrinfo(results);

    s = listen(skt, 10);
    if (s == -1) {
      printf("Error: %s\n", strerror(errno));
      close(skt);
      return 1;
    }

    while (_read_sensor(sensor, &temperature)){
        peerskt = accept(skt, NULL, NULL);
        /* Manejo del mensaje del cliente */
        request_buffer = malloc(sizeof(char) * BUFFER_SIZE);
        ok = receive_msg(peerskt, request_buffer, BUFFER_SIZE);

        /* Manejo de la respuesta al cliente */
        response_buffer = malloc(sizeof(char) * BUFFER_SIZE);
        const char *status = _parse_request(request_buffer, &user_agent);
        snprintf(header, sizeof(header), "HTTP/1.1 %s\n\n", status);
        ptr_body = stpncpy(response_buffer, header, strlen(header));
        if (strcmp(status, OK) == 0){
            if (user_agent) (printf("User-agent: %s\n", user_agent));
            free(user_agent);
            _prepare_body(template, temperature, ptr_body);
        } else{
            response_buffer[strlen(header)] = '\0';
        }
        ok = send_msg(peerskt, response_buffer, strlen(response_buffer));
        printf("%d\n", ok);
        free(response_buffer);
        free(request_buffer);
        shutdown(peerskt, SHUT_RDWR);
        close(peerskt);
    }
    
    fclose(sensor);
    fclose(template);
    shutdown(skt, SHUT_RDWR);
    close(skt);
    return 0;
}
