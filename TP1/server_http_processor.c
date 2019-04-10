#define _GNU_SOURCE
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include "server_http_processor.h"

#define BUFFER_SIZE 512
#define HEADER_SIZE 50
#define TEMP_LENGTH 50
#define GET "GET "
#define USER_AGENT "User-Agent: "
#define URI "GET /sensor "
#define OK "200 OK"
#define BAD_REQUEST "400 Bad request"
#define NOT_FOUND "404 Not found"
#define TEMPLATE_DATA "{{datos}}"

static const char* _parse_request_and_return_status(char *request, char **ptr){
    char *request_action = strstr(request, GET);
    if (!request_action || (request - request_action) != 0) return BAD_REQUEST;
    char *request_uri = strstr(request, URI);
    if (!request_uri || (request - request_action) != 0) return NOT_FOUND;
    *ptr = strstr(request, USER_AGENT);
    if (*ptr) *ptr += strlen(USER_AGENT);
    return OK; 
} 

static void _prepare_body(FILE *template, float temperature, char **response,
            const char* status){
    fseek(template, 0, SEEK_END);
	int template_len = ftell(template);
	fseek(template, 0, SEEK_SET);

    char header[HEADER_SIZE];
    snprintf(header, sizeof(header), "HTTP/1.1 %s\n\n", status);
    int header_len = strlen(header);
    char temp[TEMP_LENGTH];
    int temp_len = snprintf(temp, sizeof(temp), "%.2f", temperature);
    int response_len = temp_len + template_len + header_len + 1;

    *response = malloc(sizeof(char) * response_len);

    char* ptr_body = stpncpy(*response, header, header_len);
    if (strcmp(status, OK) != 0){
        (*response)[header_len] = '\0';
        return;
    }

    char line_buffer[BUFFER_SIZE];
    size_t total_read = 0;
    size_t first_half; 
    size_t rep_len = strlen(TEMPLATE_DATA);
    char *replacement;
    
    /* We'll look for a given substring, and replace it
    with the temperature we read from the sensor */
    while (fgets(line_buffer, BUFFER_SIZE, template)){
        size_t nread = strlen(line_buffer);
        if ((replacement = strstr(line_buffer, TEMPLATE_DATA))){
            first_half = replacement - line_buffer;
            memcpy(ptr_body, line_buffer, first_half);
            ptr_body += first_half;
            memcpy(ptr_body, temp, strlen(temp));
            ptr_body += strlen(temp);
            memcpy(ptr_body, replacement + rep_len, nread-first_half-rep_len);
            ptr_body += nread - first_half - rep_len;
            nread -= rep_len - strlen(temp);
        } else {
             memcpy(ptr_body, line_buffer, nread);
             ptr_body += nread;
        }
        total_read += nread;
    }
    *(ptr_body) = '\0';
    fseek(template, 0, SEEK_SET);
}

int http_processor_init(http_processor_t *hp, char *request_buffer,
        char **response_buffer, const char *template_path){
    if (!request_buffer || !response_buffer){
        return 1;
    }
    hp->http_template = fopen(template_path, "r");
    if (!hp->http_template){
        printf("Error: %s\n", strerror(errno));
        return 1;
    }
    hp->request = request_buffer;
    hp->response = response_buffer;
    return 0;
}

int http_processor_process(http_processor_t *hp, float temperature,
        char **user_agent_ptr){
    const char *status = _parse_request_and_return_status(hp->request,
        user_agent_ptr);
    _prepare_body(hp->http_template, temperature, hp->response, status);
    return strcmp(status, OK) == 0 ? 0 : 1;
}

void http_processor_destroy(http_processor_t *hp){
    fclose(hp->http_template);
}
