#define _GNU_SOURCE

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include "common_socket.h"
#include "common_bc.h"

#define BUFFER_SIZE 512
#define HEADER_SIZE 50
#define ARGV_SIZE 4
#define TEMP_LENGTH 10
#define GET "GET "
#define USER_AGENT "User-Agent: "
#define URI "GET /sensor "
#define OK "200 OK"
#define BAD_REQUEST "400 Bad request"
#define NOT_FOUND "404 Not found"
#define TEMPLATE_DATA "{{datos}}"
#define SENSOR_TEMP_SIZE 2

static size_t _read_sensor(FILE *sensor, float *temperature){
    uint16_t ne; // Network byte-order
    uint16_t he; // Host byte-order
    size_t elements_read = 0;

    if ((elements_read = fread(&ne, SENSOR_TEMP_SIZE, 1, sensor)) > 0){
        he = ntohs(ne); 
        *temperature = (he - 2000) / 100.0; 
    }
    return elements_read;
}

static const char* _parse_request(char *request, char **ptr){
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

static void _prepare_body(FILE *template, float temperature, char *response){
    char line_buffer[BUFFER_SIZE];
    
    size_t total_read = 0;
    size_t first_half; /* Bytes until the substring we want to replace */
    size_t rep_len = strlen(TEMPLATE_DATA);
    char *replacement;
    char temp[TEMP_LENGTH];

    snprintf(temp, sizeof(temp), "%.2f", temperature);
    /* We'll look for a given substring, and replace it
    with the temperature we read from the sensor */
    while (fgets(line_buffer, BUFFER_SIZE, template)){
        size_t nread = strlen(line_buffer);
        if ((replacement = strstr(line_buffer, TEMPLATE_DATA))){
            first_half = replacement - line_buffer;
            memcpy(response, line_buffer, first_half);
            response += first_half;
            memcpy(response, temp, strlen(temp));
            response += strlen(temp);
            memcpy(response, replacement + rep_len, nread-first_half-rep_len);
            response += nread - first_half - rep_len;
            nread -= rep_len - strlen(temp);
        } else {
             memcpy(response, line_buffer, nread);
             response += nread;
        }
        total_read += nread;
    }
    *(response) = '\0';
    fseek(template, 0, SEEK_SET);
}

static int _accept_client(socket_t *skt, FILE *template, float temperature,
                                        browser_counter_t * bc){
    char *user_agent; 
    char *request_buffer;
    char *response_buffer;
    char header[BUFFER_SIZE];
    char *ptr_body;
    int ret_code = 0;
    int ok = 0;

    request_buffer = malloc(sizeof(char) * BUFFER_SIZE);
    ok = socket_receive_msg(skt, request_buffer, BUFFER_SIZE);
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
        ok = socket_send_msg(skt, response_buffer, strlen(response_buffer));
        if (!ok){
            ret_code = 1;
            printf("Couldn't send the message to the client\n");
        }
        free(response_buffer);
        free(request_buffer);
    }     
    
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
    socket_t skt; 
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

    if (socket_init(&skt, "localhost", port, 1)){
        printf("Error: %s\n", strerror(errno));
        fclose(sensor);
        fclose(template);
        browser_counter_destroy(&bc);
        return 1;
    }

    while (_read_sensor(sensor, &temperature)){
        while (_accept_client(&skt, template, temperature, &bc)){
        }
    }
    
    fclose(sensor);
    fclose(template);
    socket_destroy(&skt);
    browser_counter_print_stats(&bc);
    browser_counter_destroy(&bc);

    return 0;
}
