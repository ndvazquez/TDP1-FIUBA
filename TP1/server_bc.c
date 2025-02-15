#include "server_bc.h"

#define BROWSER_COUNTER_INIT_SIZE 10

static void _browser_counter_resize(browser_counter_t *bc){
    field_t * new_fields = realloc(bc->fields, bc->size * 2);
    if (new_fields){
        bc->fields = new_fields;
        bc->size *= 2;
    }
}

int browser_counter_init(browser_counter_t *bc){
    bc->fields = malloc(sizeof(field_t*) * BROWSER_COUNTER_INIT_SIZE);
    if (!bc->fields){
        return 1;
    }
    bc->size = BROWSER_COUNTER_INIT_SIZE;
    bc->elements = 0;
    return 0;
}

void browser_counter_insert(browser_counter_t *bc, const char *user_agent){
    size_t browser_lenght = 0;
    while (*(user_agent+browser_lenght) != '\n') {
        browser_lenght++;
    }
    char *browser = malloc(sizeof(char) * (browser_lenght + 1));
    memcpy(browser, user_agent, browser_lenght);
    browser[browser_lenght] = '\0';

    if (bc->size == bc->elements){
        _browser_counter_resize(bc);
    }
    for (int i = 0; i < bc->elements; ++i){
        if (strcmp(bc->fields[i].user_agent, browser) == 0){
            bc->fields[i].counter++;
            free(browser);
            return;
        }
    }

    field_t field;
    field.counter = 1;
    field.user_agent = browser;
    bc->fields[bc->elements] = field;
    bc->elements++;
}

void browser_counter_print_stats(browser_counter_t *bc){
    printf("# Estadisticas de visitantes\n\n");
    for (int i = 0; i < bc->elements; ++i){
        field_t field = bc->fields[i];
        printf("* %s: %d\n", field.user_agent, field.counter);
    }
}

void browser_counter_destroy(browser_counter_t *bc){
    for (int i = 0; i < bc->elements; ++i){
        free(bc->fields[i].user_agent);
    }
    free(bc->fields);
}
