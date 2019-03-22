#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

static const char GET[] = "GET ";
static const char USER_AGENT[] = "User-Agent: ";
static const char URI[] = "GET /sensor ";
static const char OK[] = "200 OK";
static const char BAD_REQUEST[] = "400 Bad request";
static const char NOT_FOUND[] = "404 Not found";
static const char TEMPLATE_DATA[] = "{{datos}}";

size_t _read_sensor(FILE *sensor, float *temperature){
    uint16_t be;
    uint16_t le;
    size_t elements_read;

    if ((elements_read = fread(&be, sizeof(uint16_t), 1, sensor)) > 0){
        le = ntohs(be); 
        *temperature = (le - 2000) / 100.0; 
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
    // TODO: Recibir bien los parametros.
    FILE *sensor;
    FILE *template;
    FILE *request;

    size_t bytes_read;
    float temperature;

    char *user_agent = NULL; // Buffer para guardar el browser
    char request_buffer[512];
    char *response_buffer;
    char header[30];
    char *ptr;

    sensor = fopen(argv[1], "rb");
    template = fopen(argv[2], "r");

    while (_read_sensor(sensor, &temperature)){
        request = fopen(argv[3], "r"); // Esto emula los sockets
        bytes_read = fread(request_buffer, sizeof(char), 512, request);
        request_buffer[bytes_read] = '\0';

        response_buffer = malloc(sizeof(char) * 512);
        const char *status = _parse_request(request_buffer, &user_agent);
        snprintf(header, sizeof(header), "HTTP/1.1 %s\n\n", status);
        ptr = stpncpy(response_buffer, header, strlen(header));
        if (strcmp(status, OK) == 0){
            if (user_agent) (printf("User-agent: %s\n", user_agent));
            free(user_agent);
            _prepare_body(template, temperature, ptr);
        }
        printf("%s", response_buffer);
        free(response_buffer);
        fclose(request);
    }
    fclose(sensor);
    fclose(template);
    return 0;
}
