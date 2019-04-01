#define _GNU_SOURCE
#define _POSIX_C_SOURCE 200112L

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "common.h"
#include "common_bc.h"
#include "server.h"

#define BUFFER_SIZE 512
#define HEADER_SIZE 50
#define ARGV_SIZE 4
#define TEMP_LENGTH 10

static const char GET[] = "GET ";
static const char USER_AGENT[] = "User-Agent: ";
static const char URI[] = "GET /sensor ";
static const char OK[] = "200 OK";
static const char BAD_REQUEST[] = "400 Bad request";
static const char NOT_FOUND[] = "404 Not found";
static const char TEMPLATE_DATA[] = "{{datos}}";

size_t _read_sensor(FILE *sensor, float *temperature){
    uint16_t ne; // Network byte-order
    uint16_t he; // Host byte-order
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
    size_t first_half; /* Bytes until the substring we want to replace */
    size_t rep_len = strlen(TEMPLATE_DATA);
    char *replacement;
    char temp[10];

    snprintf(temp, sizeof(temp), "%.2f", temperature);
    /* We'll look for a given substring, and replace it
    with the temperature we read from the sensor */
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
    *(response) = '\0';
    free(line);
    fseek(template, 0, SEEK_SET);
}

int _accept_client(int skt, FILE *template, float temperature,
                                        browser_counter_t * bc){
    char *user_agent; 
    char *request_buffer;
    char *response_buffer;
    char header[BUFFER_SIZE];
    char *ptr_body;
    int ret_code = 0;
    int peerskt, ok = 0;

    peerskt = accept(skt, NULL, NULL);
    if (peerskt == -1){
        printf("Error: %s\n", strerror(errno));
        ret_code = 1;
    } else{
        request_buffer = malloc(sizeof(char) * BUFFER_SIZE);
        ok = receive_msg(peerskt, request_buffer, BUFFER_SIZE);
        shutdown(peerskt, SHUT_RD);
        if (!ok){
            printf("Didn't receive anything, next!\n");
            ret_code = 1;
        } else{
            response_buffer = malloc(sizeof(char) * BUFFER_SIZE);
            const char *status = _parse_request(request_buffer, &user_agent);
            snprintf(header, sizeof(header), "HTTP/1.1 %s\n\n", status);
            ptr_body = stpncpy(response_buffer, header, strlen(header));
            if (strcmp(status, OK) == 0){
                if (user_agent) {
                    browser_counter_insert(bc, user_agent);
                }
                free(user_agent);
                _prepare_body(template, temperature, ptr_body);
            } else{
                response_buffer[strlen(header)] = '\0';
                ret_code = 1;
            }
            ok = send_msg(peerskt, response_buffer, strlen(response_buffer));
            shutdown(peerskt, SHUT_WR);
            if (!ok){
                ret_code = 1;
                printf("Couldn't send the message to the client\n");
            }
            free(response_buffer);
            free(request_buffer);
        }     
    }
    close(peerskt);
    return ret_code;
}

int main(int argc, char *argv[]){
    if (argc < ARGV_SIZE){
        printf("Uso:\n./server <puerto> <input> [<template>]\n");
        return 1;
    }

    FILE *sensor;
    FILE *template;
    char *port = argv[1];
    int s = 0;
    int skt = 0;
    struct addrinfo hints;
    struct addrinfo *res;
    float temperature;
    browser_counter_t bc;

    sensor = fopen(argv[2], "rb");
    if (!sensor){
        printf("Error: %s\n", strerror(errno));
        return 1;
    }

    template = fopen(argv[3], "r");
    if (!template){
        printf("Error: %s\n", strerror(errno));
        fclose(sensor);
        return 1;
    }

    if (browser_counter_init(&bc)){
        printf("Error: %s\n", strerror(errno));
        fclose(sensor);
        fclose(template);
        return 1;
    }

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    s = getaddrinfo("localhost", port, &hints, &res);
    if (s != 0) { 
        printf("Error in getaddrinfo: %s\n", gai_strerror(s));
        freeaddrinfo(res);
        fclose(sensor);
        fclose(template);
        return 1;
    }

    skt = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (skt == -1) {
        printf("Error: %s\n", strerror(errno));
        freeaddrinfo(res);
        fclose(sensor);
        fclose(template);
        return 1;
    }

    int val = 1;
    s = setsockopt(skt, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));
    if (s == -1) {
        printf("Error: %s\n", strerror(errno));
        shutdown(skt, SHUT_RDWR);
        close(skt);
        freeaddrinfo(res);
        fclose(sensor);
        fclose(template);
        return 1;
    }

    s = bind(skt, res->ai_addr, res->ai_addrlen);
    if (s == -1) {
        printf("Error: %s\n", strerror(errno));
        shutdown(skt, SHUT_RDWR);
        close(skt);
        freeaddrinfo(res);
        fclose(sensor);
        fclose(template);
        return 1; 
    }

    freeaddrinfo(res);

    s = listen(skt, 10);
    if (s == -1) {
        printf("Error: %s\n", strerror(errno));
        shutdown(skt, SHUT_RDWR);
        close(skt);
        fclose(sensor);
        fclose(template);
        return 1;
    }
    
    while (_read_sensor(sensor, &temperature)){
        while (_accept_client(skt, template, temperature, &bc)){
        }
    }
    
    fclose(sensor);
    fclose(template);
    shutdown(skt, SHUT_RDWR);
    close(skt);

    browser_counter_print_stats(&bc);
    browser_counter_destroy(&bc);

    return 0;
}

