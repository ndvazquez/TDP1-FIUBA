#ifndef SERVER_BC_H
#define SERVER_BC_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct field{
    char *user_agent;
    int counter;
} field_t;

typedef struct browser_counter{
    field_t *fields;
    size_t size;
    size_t elements;
} browser_counter_t;

//Initializes the structure.
int browser_counter_init(browser_counter_t *bc);
//This function extracts the browser name from a HTTP Request as a key.
//Inserts a new key in the Browser Counter and sets its counter to 1.
//If it's already in the Browser Counter, it increases the field counter by 1.
void browser_counter_insert(browser_counter_t *bc, const char *user_agent);
//Prints the stats for each browser inside Browser Counter.
void browser_counter_print_stats(browser_counter_t *bc);
//Destroys the Browser Counter.
void browser_counter_destroy(browser_counter_t *bc);

#endif
