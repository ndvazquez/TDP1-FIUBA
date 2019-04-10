#ifndef COMMON_HTTP_PROCESSOR_H
#define COMMON_HTTP_PROCESSOR_H

#include <stdio.h>

typedef struct http_processor{
    char *request;
    char **response;
    FILE *http_template;
} http_processor_t;

// Initializes the HttpProcessor.
int http_processor_init(http_processor_t *hp, char *request_buffer,
        char **response_buffer, const char *template_path);
// This function process a given HTTP request when initialized, and then
// prepares a response for the request.
// If it finds a user_agent while parsing the HTTP request, it saves
// that on user_agent_ptr.
// It's the responsability of the user to free the memory allocated for
// response_buffer.
int http_processor_process(http_processor_t *hp, float temperature,
        char **user_agent_ptr);

void http_processor_destroy(http_processor_t *hp);

#endif
