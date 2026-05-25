#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdio.h>
#include "request.h"

static int find_header_end(const char *raw, size_t length)
{
    for (size_t i = 0; i + 3 < length; i++)
    {
        if (raw[i] == '\r' && raw[i + 1] == '\n' && raw[i + 2] == '\r' && raw[i + 3] == '\n')
        {
            return (int)i + 4;
        }
    }
    return -1;
}

static void trim_left(char **s)
{
    while (**s == ' ' || **s == '\t')
    {
        (*s)++;
    }
}

static void trim_right(char *s)
{
    int len = strlen(s);
    while (len > 0 && (s[len - 1] == ' ' || s[len - 1] == '\t' || s[len - 1] == '\r' || s[len - 1] == '\n'))
    {
        s[len - 1] = '\0';
        len--;
    }
}

int request_parse(const char *raw, size_t raw_length, Request *request)
{
    memset(request, 0, sizeof(Request));

    int header_end = find_header_end(raw, raw_length);
    if (header_end == -1)
    {
        return -1;
    }

    size_t header_length = header_end - 4;
    char *header_block = malloc(header_length + 1);
    if (!header_block)
    {
        return -1;
    }

    memcpy(header_block, raw, header_length);
    header_block[header_length] = '\0';

    char *saveptr = NULL;
    char *line = strtok_r(header_block, "\r\n", &saveptr);

    if (!line)
    {
        free(header_block);
        return -1;
    }

    char method[16];
    char path[MAX_PATH];
    char version[MAX_VERSION];

    if (sscanf(line, "%15s %1023s %31s", method, path, version) != 3)
    {
        free(header_block);
        return -1;
    }

    request->method = http_method_from_string(method);
    strncpy(request->path, path, MAX_PATH - 1);
    strncpy(request->version, version, MAX_VERSION - 1);

    while ((line = strtok_r(NULL, "\r\n", &saveptr)) != NULL)
    {
        if (request->header_count >= MAX_HEADERS)
        {
            break;
        }

        char *colon = strchr(line, ':');
        if (!colon)
        {
            continue;
        }

        *colon = '\0';

        char *key = line;
        char *value = colon + 1;

        trim_left(&value);
        trim_right(key);
        trim_right(value);

        strncpy(request->headers[request->header_count].key, key, MAX_HEADER_KEY - 1);
        strncpy(request->headers[request->header_count].value, value, MAX_HEADER_VALUE - 1);

        request->header_count++;
    }

    size_t body_length = raw_length - header_end;
    request->body_length = body_length;

    if (body_length > 0)
    {
        request->body = malloc(body_length + 1);
        if (!request->body)
        {
            free(header_block);
            return -1;
        }

        memcpy(request->body, raw + header_end, body_length);
        request->body[body_length] = '\0';
    }

    free(header_block);
    return 0;
}

const char *request_get_header(Request *request, const char *key)
{
    for (int i = 0; i < request->header_count; i++)
    {
        if (strcasecmp(request->headers[i].key, key) == 0)
        {
            return request->headers[i].value;
        }
    }
    return NULL;
}

void request_free(Request *request)
{
    if (request->body)
    {
        free(request->body);
        request->body = NULL;
    }
}
