#ifndef __USER_H
#define __USER_H_
#include "http.h"

#define MAX_SIZE_STATION 512

typedef struct {
    unsigned id;
    char* name;
} user_station_t;

extern user_station_t s[];

typedef struct user_data_t {
    char url[MAX_URL_SIZE];
    char line_filter;
} user_data_t;

/* public api */
int user_input_validate(char* argv[], int argc, user_data_t* self);

#endif /* __INPUT_H_ */