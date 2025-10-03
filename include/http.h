#ifndef __HTTP_H_
#define __HTTP_H_
#include <stddef.h>

#define MAX_URL_SIZE 512

typedef enum {
    STATUS_OK,
    STATUS_FAIL,
    STATUS_EXIT
} status_code_e;

/* forward declaration */
typedef struct user_data_t user_data_t; 

typedef struct {
    char* data;
    size_t size;
} http_membuf_t;

size_t http_write_data(void* buf, size_t size, size_t nmemb, void* userp);
char* http_get(user_data_t* ud);
void http_metro_departures(const char* http_response, user_data_t* ud);

#endif /* __HTTP_H_ */