#ifndef COMMON_BC_H
#define COMMON_BC_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

struct field;
typedef struct field field_t;
struct browser_counter;
typedef struct browser_counter browser_counter_t;

//Allocates memory for a Browser Counter and
//returns a pointer to the structure.
browser_counter_t* browser_counter_create();
//Inserts a new key in the Browser Counter and sets its counter to 1.
//If it's already in the Browser Counter, it increases the field counter by 1.
void browser_counter_insert(browser_counter_t *bc, char *user_agent);
//If the Browser Counter gets full, it doubles its size.
void _browser_counter_resize(browser_counter_t *bc);
//Prints the stats for each browser inside Browser Counter.
void browser_counter_print_stats(browser_counter_t *bc);
//Destroys the Browser Counter.
void browser_counter_destroy(browser_counter_t *bc);

#endif

