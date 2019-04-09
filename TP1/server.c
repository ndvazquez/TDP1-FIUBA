#include <stdlib.h>
#include <errno.h>
#include "common_socket.h"
#include "common_bc.h"
#include "common_http_processor.h"
#include "common_sensor_reader.h"

#define BUFFER_SIZE 2048
#define ARGV_SIZE 4

static int _serve(socket_t *skt, const char *template_path,
                    float temperature, browser_counter_t *bc){ 
    int ret_code = 0;

    socket_t peerskt;
    socket_init(&peerskt);
    int s = socket_accept(skt, &peerskt);
    if (s == -1){
        socket_destroy(&peerskt);
        ret_code = 1;
        return ret_code;
    }

    char *request_buffer = malloc(sizeof(char) * BUFFER_SIZE);
    int ok = socket_receive(&peerskt, request_buffer, BUFFER_SIZE);
    if (ok == -1 || ok == 0){
        free(request_buffer);
        socket_destroy(&peerskt);
        ret_code = 1;
    } else{
        request_buffer[ok] = '\0';
        char *response_buffer = malloc(sizeof(char) * BUFFER_SIZE);
        http_processor_t hp;
        http_processor_init(&hp, request_buffer,
            response_buffer, template_path);
        char *user_agent = NULL;
        ret_code = http_processor_process(&hp, temperature, &user_agent);
        if (user_agent){
            browser_counter_insert(bc, user_agent);
            free(user_agent);
        }

        ok = socket_send(&peerskt, response_buffer, strlen(response_buffer));
        if (ok == -1 || ok == 0){
            ret_code = 1;
        }
        http_processor_destroy(&hp);
        free(response_buffer);
        free(request_buffer);
        socket_destroy(&peerskt);
    }     
    
    return ret_code;
}

int main(int argc, char *argv[]){
    if (argc < ARGV_SIZE){
        printf("Uso:\n./server <puerto> <input> [<template>]\n");
        return 1;
    }

    const char *sensor_path = argv[2];
    sensor_reader_t sr;
    if (sensor_reader_init(&sr, sensor_path)){
        printf("Error: %s\n", strerror(errno));
        return 1;
    }
    browser_counter_t bc;
    if (browser_counter_init(&bc)){
        printf("Error: %s\n", strerror(errno));
        sensor_reader_destroy(&sr);
        return 1;
    }

    char *port = argv[1];
    socket_t skt;
    socket_init(&skt);
    if (socket_bind_and_listen(&skt, NULL, port)){
        printf("Error: %s\n", strerror(errno));
        sensor_reader_destroy(&sr);
        browser_counter_destroy(&bc);
        return 1;
    }

    float temperature;
    const char *template_path = argv[3];
    while (sensor_reader_get_temperature(&sr, &temperature)){
        while (_serve(&skt, template_path, temperature, &bc)){
        }
    }
    
    sensor_reader_destroy(&sr);
    socket_destroy(&skt);
    browser_counter_print_stats(&bc);
    browser_counter_destroy(&bc);

    return 0;
}
