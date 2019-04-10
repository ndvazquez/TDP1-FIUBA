#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>
#include "common_socket.h"
#include "common_receiver.h"
#include "server_bc.h"
#include "server_http_processor.h"
#include "server_sensor_reader.h"

#define ARGV_SIZE 4

static int _serve(socket_t *skt, const char *template_path,
                    float temperature, browser_counter_t *bc){ 
    socket_t peerskt;
    socket_init(&peerskt);
    int s = socket_accept(skt, &peerskt);
    if (s == -1){
        socket_destroy(&peerskt);
        return 1;
    }

    receiver_t rcv;
    receiver_init(&rcv);
    char *request_buffer = NULL;
    int ok = receiver_receive_data(&rcv, &peerskt, &request_buffer);
    if (ok == -1 || ok == 0){
        if (request_buffer) free(request_buffer);
        socket_destroy(&peerskt);
        return 1;
    }

    char *response_buffer = NULL;
    http_processor_t hp;
    http_processor_init(&hp, request_buffer, &response_buffer, template_path);
    char *user_agent = NULL;
    int status = http_processor_process(&hp, temperature, &user_agent);
    if (user_agent){
        browser_counter_insert(bc, user_agent);
    }

    ok = socket_send(&peerskt, response_buffer, strlen(response_buffer));
    if (status != 0 || ok == -1 || ok == 0){
        http_processor_destroy(&hp);
        free(response_buffer);
        free(request_buffer);
        socket_destroy(&peerskt);
        return 1;
    }
    http_processor_destroy(&hp);
    free(response_buffer);
    free(request_buffer);
    socket_destroy(&peerskt);

    return 0;
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
