#ifndef REQUEST_H
#define REQUEST_H

#include <stddef.h>
#include "http.h"

#define MAX_HEADERS 64
#define MAX_HEADER_KEY 128
#define MAX_HEADER_VALUE 512
#ifdef MAX_PATH
#undef MAX_PATH
#endif
#define MAX_PATH 1024
#define MAX_VERSION 32

typedef struct
{
    char key[MAX_HEADER_KEY];
    char value[MAX_HEADER_VALUE];
} Header;

typedef struct
{
    HttpMethod method;
    char path[MAX_PATH];
    char version[MAX_VERSION];
    Header headers[MAX_HEADERS];
    int header_count;
    char *body;
    size_t body_length;
} Request;

int request_parse(const char *raw, size_t raw_length, Request *request);
const char *request_get_header(Request *request, const char *key);
void request_free(Request *request);

#endif
