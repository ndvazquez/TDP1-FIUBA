#ifndef COMMON_SENSOR_READER_H
#define COMMON_SENSOR_READER_H

#include <stdio.h>

typedef struct sensor_reader{
    FILE *sensor;
} sensor_reader_t;

int sensor_reader_get_temperature(sensor_reader_t *sr, float *temperature);
int sensor_reader_init(sensor_reader_t *sr,const char *sensor_path);
void sensor_reader_destroy(sensor_reader_t *sr);

#endif
