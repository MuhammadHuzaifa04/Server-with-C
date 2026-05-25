#ifndef RESPONSE_H
#define RESPONSE_H

#include <stddef.h>

typedef struct
{
    int status_code;
    char content_type[128];
    char *body;
    size_t body_length;
} Response;

void response_init(Response *response);
void response_set_status(Response *response, int status_code);
void response_set_content_type(Response *response, const char *content_type);
void response_set_body(Response *response, const char *body);
void response_set_body_n(Response *response, const char *body, size_t length);
int response_send(int client_fd, Response *response);
void response_free(Response *response);

#endif
