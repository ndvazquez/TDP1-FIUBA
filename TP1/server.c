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

void _prepare_response(FILE *template, float temperature){
    char *body;
    char *line = NULL;
    size_t getline_len = 0;
    ssize_t nread;
    size_t total_read = 0;
    size_t first_half;
    size_t rep_len = strlen(TEMPLATE_DATA);
    char *replacement;
    char temp[10];

    snprintf(temp, 10, "%.2f", temperature);
    body = malloc(sizeof(char) * 512);

    while ((nread = getline(&line, &getline_len, template)) > 0){
        if ((replacement = strstr(line, TEMPLATE_DATA))){
            first_half = replacement - line;
            memcpy(body, line, first_half);
            body += first_half;
            memcpy(body, temp, strlen(temp));
            body += strlen(temp);
            memcpy(body, replacement + rep_len, nread - first_half - rep_len);
            body += nread - first_half - rep_len;
            nread -= rep_len - strlen(temp);
        } else {
             memcpy(body, line, nread);
             body += nread;
        }
        total_read += nread;
    }
    body -= total_read;
    body[total_read] = '\0';
    free(line);
    printf("%s\n", body);
    free(body);
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

    sensor = fopen(argv[1], "rb");
    template = fopen(argv[2], "r");

    while (_read_sensor(sensor, &temperature)){
        request = fopen(argv[3], "r"); // Esto emula los requests recibidos por sockets
        bytes_read = fread(request_buffer, sizeof(char), 512, request);
        request_buffer[bytes_read] = '\0';
        const char *response = _parse_request(request_buffer, &user_agent);
        printf("HTTP/1.1 %s\n", response);
        if (strcmp(response, OK) == 0){
            if (user_agent) (printf("User-agent: %s\n", user_agent));
            free(user_agent);
            _prepare_response(template, temperature);
        }
        fclose(request);
    }
    fclose(sensor);
    fclose(template);
    return 0;
}
