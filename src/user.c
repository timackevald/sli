#include "user.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define N_STATIONS 100

/* to do fix list */
user_station_t s[N_STATIONS] = {
    {9110, "Abrahamsberg"},
    {9300, "Akalla"},
    {9282, "Alby"},
    {9112, "Alvik"},
    {9293, "Aspudden"},
    {9291, "Axelsberg"},
    {9141, "Bagarmossen"},
    {9163, "Bandhagen"},
    {9202, "Bergshamra"},
    {9143, "Björkhagen"},
    {9105, "Blackeberg"},
    {9187, "Blåsut"},
    {9289, "Bredäng"},
    {9109, "Brommaplan"},
    {9201, "Danderyds sjukhus"},
    {9324, "Duvbo"},
    {9167, "Enskede gård"},
    {9181, "Farsta"},
    {9180, "Farsta strand"},
    {9283, "Fittja"},
    {9115, "Fridhemsplan"},
    {9260, "Fruängen"},
    {9193, "Gamla stan"},
    {9221, "Gärdet"},
    {9168, "Globen"},
    {9183, "Gubbängen"},
    {9189, "Gullmarsplan"},
    {9160, "Hagsätra"},
    {9262, "Hägerstensåsen"},
    {9309, "Hallonbergen"},
    {9281, "Hallunda"},//7385
    {9144, "Hammarbyhöjden"},
    {9320, "Hjulsta"},
    {9295, "Hornstull"},
    {9327, "Huvudsta"},
    {9301, "Husby"},
    {9101, "Hässelby gård"},
    {9100, "Hässelby strand"},
    {9162, "Högdalen"},
    {9182, "Hökarängen"}, //9179
    {9119, "Hötorget"},
    {9106, "Islandstorget"},
    {9102, "Johannelund"},
    {9222, "Karlaplan"},
    {9302, "Kista"},
    {9113, "Kristineberg"},
    {9340, "Kungsträdgården"},
    {9142, "Kärrtorp"},
    {9294, "Liljeholmen"},
    {9297, "Mariatorget"},
    {9284, "Masmo"},
    {9191, "Medborgarplatsen"},
    {9264, "Midsommarkransen"},
    {9200, "Mörby centrum"},
    {9290, "Mälarhöjden"},
    {9304, "Näckrosen"},
    {9280, "Norsborg"},
    {9117, "Odenplan"},
    {9104, "Råcksta"},
    {9181, "Rådhuset"},
    {9309, "Rådmansgatan"},
    {9161, "Rågsved"},
    {9322, "Rinkeby"},
    {9323, "Rissne"},
    {9220, "Ropsten"},
    {9186, "Sandsborg"},
    {9116, "Sankt Eriksplan"},
    {9288, "Sätra"},
    {9190, "Skanstull"},
    {9140, "Skarpnäck"},
    {9287, "Skärholmen"},
    {9188, "Skärmarbrink"},
    {9185, "Skogskyrkogården"},
    {9192, "Slussen"},
    {9166, "Sockenplan"},
    {9305, "Solna centrum"},
    {9326, "Solna strand"},
    {9307, "Stadshagen"},
    {9205, "Stadion"},
    {9111, "Stora Mossen"},
    {9164, "Stureby"},
    {9325, "Sundbyberg"},
    {9165, "Svedmyra"},
    {9001, "T-Centralen"},
    {9184, "Tallkrogen"},
    {9204, "Tekniska högskolan"},
    {9263, "Telefonplan"},
    {9321, "Tensta"},
    {9114, "Thorildsplan"},
    {9203, "Universitetet"},
    {9103, "Vällingby"},
    {9286, "Vårberg"},
    {9285, "Vårby gård"},
    {9261, "Västertorp"},
    {9306, "Västra skogen"},
    {9296, "Zinkensdamm"},
    {9108, "Åkeshov"},
    {9107, "Ängbyplan"},
    {9292, "Örnsberg"},
    {9206, "Östermalmstorg"}
};

// In src/user.c

int user_input_validate(char* argv[], int argc, user_data_t* self) {

    // 1. Check for minimum arguments (at least one station word)
    if (argc <= 1) {
        printf("%s <station name> [g|r|b]\n", argv[0]);
        return STATUS_EXIT;
    }

    // 2. Determine if a line filter is present
    int is_filtered = 0;
    char* last_arg = argv[argc - 1];

    if (strcmp(last_arg, "g") == 0 || 
        strcmp(last_arg, "r") == 0 || 
        strcmp(last_arg, "b") == 0) {
        
        is_filtered = 1;
        self->line_filter = last_arg[0]; // Store the filter ('g', 'r', or 'b')
    } else {
        self->line_filter = 'a'; // Store 'a' (for all) if no filter is used
    }
    
    int station_name_argc = is_filtered ? argc - 1 : argc;

    // Check if a station name was actually provided (e.g., if user only typed "g")
    if (station_name_argc <= 1) {
        printf("%s <station name> [g|r|b]\n", argv[0]);
        return STATUS_FAIL; 
    }

    // 3. Validate input contains no digits for the station name
    for (int i = 1; i < station_name_argc; i++) {
        for (char* ptr = argv[i]; *ptr != '\0'; ptr++) {
            if (isdigit((unsigned char)*ptr)) {
                printf("Invalid input in %s, contains digit!\n", argv[i]);
                return STATUS_FAIL;
            }
        }
    }

    // 4. Concatenate arguments into one station name (excluding the filter)
    // Calculate total length needed for station name
    size_t len_station_name = 0;
    for (int i = 1; i < station_name_argc; i++) {
        len_station_name += strlen(argv[i]);
    }
    len_station_name += station_name_argc; // Max spaces + null terminator

    // Allocate memory for concatenated station name
    char* user_station = (char*)malloc(len_station_name);
    if (!user_station) {
        fprintf(stderr, "Memory allocation failed\n");
        return STATUS_EXIT;
    }

    // Concatenate all station name arguments
    strcpy(user_station, argv[1]);
    if (station_name_argc > 2) {
        for (int i = 2; i < station_name_argc; i++) {
            strcat(user_station, " ");
            strcat(user_station, argv[i]);
        }
    }

    // 5. Search for station in list
    unsigned station_id = 0;
    int found = 0;
    for (int i = 0; i < N_STATIONS; i++) {
        user_station_t* us = &s[i];
        if (strcmp(user_station, us->name) == 0) {
            station_id = us->id;
            found = 1;
            break;
        }
    }

    if (!found) {
        printf("Station '%s' not found!\n", user_station);
        free(user_station);
        user_station = NULL;
        return STATUS_FAIL;
    }

    // 6. Build URL with station ID
    snprintf(self->url, MAX_URL_SIZE, 
             "https://transport.integration.sl.se/v1/sites/%u/departures", 
             station_id);
    
    // Cleanup
    free(user_station);
    user_station = NULL;

    return STATUS_OK;
}