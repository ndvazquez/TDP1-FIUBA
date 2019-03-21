#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

static const char GET[] = "GET ";
static const char USER_AGENT[] = "User-Agent: ";
static const char URI[] = "/sensor ";
static const char OK[] = "200 OK";
static const char BAD_REQUEST[] = "400 Bad request";
static const char NOT_FOUND[] = "404 Not found";
//static const char TEMPLATE_DATA[] = "{datos}";

size_t _read_sensor(FILE *sensor, float *temperature){
    uint16_t be;
    uint16_t le;
    size_t elements_read;

    if ((elements_read = fread(&be, sizeof(uint16_t), 1, sensor)) > 0){
        le = ntohs(be); // Paso big-endian a little-endian
        *temperature = (le - 2000) / 100.0; // Calculo la temperatura
    }
    return elements_read;
}

const char* _parse_request(char *request, char **ptr){
    char *request_action;
    char *request_uri;
    char *request_user_agent;
    size_t browser_lenght = 0;

    request_action = strstr(request, GET);
    if (!request_action) return BAD_REQUEST;
    request_uri = strstr(request, URI);
    if (!request_uri) return NOT_FOUND;
    request_user_agent = strstr(request, USER_AGENT);
    // TODO: Usar getline para esto, soy un choto.
    if (request_user_agent) {
        request_user_agent += strlen(USER_AGENT);
        // TODO: No incrementar el puntero, sumarle el browser_lenght directamente.
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

// char* _prepare_response(FILE *template, float temperature){
//     char *response;
//     char *line;
//     size_t getline_len = 0;
//     ssize_t nread;

//     response = malloc(sizeof(char) * 512);
//     while (nread = getline(&line, &getline_len, template) > 0){

//     }
// }

// Cargar el template como archivo, procesarlo con getline y usar strstr
// para encontrar {datos}, reemplazar por temperature y ver si tener todo el template
// en memoria, o si es choto.
int main(int argc, char *argv[]){
    // TODO: Recibir bien los parametros.
    FILE *sensor;
    //FILE *template;
    FILE *request;
    size_t bytes_read;
    float temperature;
    char *user_agent = NULL; // Buffer para guardar el browser
    char request_buffer[512];

    sensor = fopen(argv[1], "rb");
    //template = fopen(argv[2], "r");

    while (_read_sensor(sensor, &temperature)){
        request = fopen(argv[3], "r"); // Esto emula los requests recibidos por sockets
        bytes_read = fread(request_buffer, sizeof(char), 512, request);
        request_buffer[bytes_read] = '\0';
        const char *response = _parse_request(request_buffer, &user_agent);
        printf("STATUS: %s\n", response);
        if (strcmp(response, OK) == 0){
            if (user_agent) (printf("User-agent: %s\n", user_agent));
            free(user_agent);
            printf("Temperature: %.2f\n", temperature);
        }
        fclose(request);
    }
    fclose(sensor);
    return 0;
}
