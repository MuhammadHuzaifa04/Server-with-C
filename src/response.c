#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#ifdef _WIN32
#include <winsock2.h>
#else
#include <unistd.h>
#endif
#include "response.h"

static int send_all(int client_fd, const char *data, size_t length)
{
    size_t sent_total = 0;

    while (sent_total < length)
    {
#ifdef _WIN32
        int sent = send((SOCKET)client_fd, data + sent_total, (int)(length - sent_total), 0);
#else
        ssize_t sent = write(client_fd, data + sent_total, length - sent_total);
#endif

        if (sent <= 0)
        {
            return -1;
        }

        sent_total += (size_t)sent;
    }

    return 0;
}

static const char *reason_phrase(int status_code)
{
    switch (status_code)
    {
    case 200:
        return "OK";
    case 201:
        return "Created";
    case 400:
        return "Bad Request";
    case 404:
        return "Not Found";
    case 405:
        return "Method Not Allowed";
    case 500:
        return "Internal Server Error";
    default:
        return "OK";
    }
}

void response_init(Response *response)
{
    response->status_code = 200;
    strcpy(response->content_type, "text/plain");
    response->body = NULL;
    response->body_length = 0;
}

void response_set_status(Response *response, int status_code)
{
    response->status_code = status_code;
}

void response_set_content_type(Response *response, const char *content_type)
{
    strncpy(response->content_type, content_type, sizeof(response->content_type) - 1);
}

void response_set_body(Response *response, const char *body)
{
    response_set_body_n(response, body, strlen(body));
}

void response_set_body_n(Response *response, const char *body, size_t length)
{
    if (response->body)
    {
        free(response->body);
    }

    response->body = malloc(length + 1);
    if (!response->body)
    {
        response->body_length = 0;
        return;
    }

    memcpy(response->body, body, length);
    response->body[length] = '\0';
    response->body_length = length;
}

int response_send(int client_fd, Response *response)
{
    char header[1024];

    int header_length = snprintf(
        header,
        sizeof(header),
        "HTTP/1.1 %d %s\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %zu\r\n"
        "Connection: close\r\n"
        "\r\n",
        response->status_code,
        reason_phrase(response->status_code),
        response->content_type,
        response->body_length);

    if (send_all(client_fd, header, (size_t)header_length) < 0)
    {
        return -1;
    }

    if (response->body && response->body_length > 0)
    {
        if (send_all(client_fd, response->body, response->body_length) < 0)
        {
            return -1;
        }
    }

    return 0;
}

void response_free(Response *response)
{
    if (response->body)
    {
        free(response->body);
        response->body = NULL;
    }
    response->body_length = 0;
}
