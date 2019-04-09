
#include <arpa/inet.h>
#include "common_sensor_reader.h"

#define SENSOR_TEMP_SIZE 2

int sensor_reader_get_temperature(sensor_reader_t *sr, float *temperature){
    uint16_t ne; // Network byte-order
    uint16_t he; // Host byte-order
    int elements_read = 0;

    if ((elements_read = fread(&ne, SENSOR_TEMP_SIZE, 1, sr->sensor)) > 0){
        he = ntohs(ne); 
        *temperature = (he - 2000) / 100.0; 
    }
    return elements_read;
}

int sensor_reader_init(sensor_reader_t *sr,const char *sensor_path){
    sr->sensor =  fopen(sensor_path, "rb");
    if (!sr->sensor){
        return 1;
    }
    return 0;
}

void sensor_reader_destroy(sensor_reader_t *sr){
    if (sr->sensor){
        fclose(sr->sensor);
    }
}

