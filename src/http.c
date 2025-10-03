#include "http.h"
#include "user.h"
#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"

char* http_get(user_data_t* ud) {
    
    CURL *curl = curl_easy_init();
    if (!curl) {
        fprintf(stderr, "Curl returned NULL\n");
        return NULL;
    }

    http_membuf_t chunk = {0};

    curl_easy_setopt(curl, CURLOPT_URL, ud->url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, http_write_data);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&chunk);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        fprintf(stderr, "Curl perform failed: %s\n", curl_easy_strerror(res));
        curl_easy_cleanup(curl);
        /*if (chunk.data) {
            free(chunk.data);
        }*/
        return NULL;
    }

    curl_easy_cleanup(curl);

    return chunk.data;
}

size_t http_write_data(void *buffer, size_t size, size_t nmemb, void *userp) {
    size_t bytes = size * nmemb;
    printf("Received chunk: %zu bytes\n", bytes);

    http_membuf_t* mem_t = (http_membuf_t*)userp;
    
    // Realloc to grow buffer (add 1 for null terminator)
    char *ptr = (char*)realloc(mem_t->data, mem_t->size + bytes + 1);
    if (!ptr) {
        fprintf(stderr, "Realloc failed in http_write_data\n");
        return 0; // Tell curl to abort
    }
    
    mem_t->data = ptr;
    memcpy(mem_t->data + mem_t->size, buffer, bytes);
    mem_t->size += bytes;
    mem_t->data[mem_t->size] = '\0';
    
    return bytes;
}

// In src/http.c

void http_metro_departures(const char* http_response, user_data_t* ud) {
    if (!http_response) {
        fprintf(stderr, "No HTTP response provided\n");
        return;
    }

    cJSON* root = cJSON_Parse(http_response);
    if (!root) {
        fprintf(stderr, "Error parsing JSON\n");
        return;
    }

    cJSON* departures = cJSON_GetObjectItem(root, "departures");
    if (!departures) {
        fprintf(stderr, "No departures found\n");
        cJSON_Delete(root);
        return;
    }

    int dep_count = cJSON_GetArraySize(departures);
    for (int i = dep_count; i >= 0; i--) {
        cJSON* dep = cJSON_GetArrayItem(departures, i);
        cJSON* line = cJSON_GetObjectItem(dep, "line");
        if (!line) continue;

        cJSON* transport_mode = cJSON_GetObjectItem(line, "transport_mode");
        cJSON* designation = cJSON_GetObjectItem(line, "designation");
        
        // 1. Filter: Must be a METRO and have a line designation
        if (!transport_mode || strcmp(transport_mode->valuestring, "METRO") != 0) continue;
        if (!designation) continue; 
        
        const char* line_designation = designation->valuestring;

        // 2. Apply the 'g', 'r', 'b' line filter if set
        if (ud->line_filter != 'a') {
            int matches = 0;
            
            // Note: Line designations are strings (e.g., "17", "13")
            switch (ud->line_filter) {
                case 'g': // Green line: 17, 18, 19
                    if (strcmp(line_designation, "17") == 0 || 
                        strcmp(line_designation, "18") == 0 || 
                        strcmp(line_designation, "19") == 0) matches = 1;
                    break;
                case 'r': // Red line: 13, 14
                    if (strcmp(line_designation, "13") == 0 || 
                        strcmp(line_designation, "14") == 0) matches = 1;
                    break;
                case 'b': // Blue line: 10, 11
                    if (strcmp(line_designation, "10") == 0 || 
                        strcmp(line_designation, "11") == 0) matches = 1;
                    break;
            }
            if (!matches) continue; // Skip departure if it doesn't match the requested line
        }

        // 3. Extract and display remaining fields
        cJSON* scheduled = cJSON_GetObjectItem(dep, "scheduled");
        cJSON* expected = cJSON_GetObjectItem(dep, "expected");
        cJSON* destination = cJSON_GetObjectItem(dep, "destination");
        cJSON* display = cJSON_GetObjectItem(dep, "display"); // This is the minutes to arrival

        char time_s[9];
        char time_e[9];
        sscanf(scheduled->valuestring, "%*10cT%8c", time_s);
        sscanf(expected->valuestring, "%*10cT%8c", time_e);
        time_s[8] = '\0';
        time_e[8] = '\0';

        printf("Line %s: %s\n", line_designation ? line_designation : "N/A", destination ? destination->valuestring : "N/A");
        
        // Show the requested "displayed minutes" for departure
        printf("Arriving in: %s minutes\n", display ? display->valuestring : "N/A");
        
        printf("Scheduled: %s\n", time_s);
        printf("Expected:  %s\n\n", time_e);
    }

    cJSON_Delete(root);
}
