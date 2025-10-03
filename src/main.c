/*
    DONE fix list with new station ID
    TODO wrap all functions as int and return codes 
    TODO move structs into correct h files
    TODO show displayed minutes for depture too 
*/
#include "user.h"
#include "http.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[]) {

    user_data_t ud = {0};

    if (user_input_validate(argv, argc, &ud) != STATUS_OK) {
        printf("Validation failed.\n");
        return STATUS_EXIT;
    }

    if (strlen(ud.url) == 0) {
        printf("URL not set\n");
        return STATUS_EXIT;
    }
    

    char* http_response = http_get(&ud);
    if (http_response) {
        http_metro_departures(http_response, &ud);
        free(http_response);
    } else {
        fprintf(stderr, "Failed to get departures\n");
    }
    
    /*free(http_response);*/
    
    return STATUS_OK;
}