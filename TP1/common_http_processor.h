#ifndef COMMON_HTTP_PROCESSOR_H
#define COMMON_HTTP_PROCESSOR_H

#include <stdio.h>

typedef struct http_processor{
    char *request;
    char *response;
    FILE *http_template;
} http_processor_t;

int http_processor_init(http_processor_t *hp, char *request_buffer,
        char *response_buffer, const char *template_path);

int http_processor_process(http_processor_t *hp, float temperature,
        char **user_agent_ptr);

void http_processor_destroy(http_processor_t *hp);

#endif
