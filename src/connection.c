#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <winsock2.h>
#include <io.h>
#include "connection.h"

#define MAX_REQUEST_SIZE 65536

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

static size_t extract_content_length(const char *raw, int header_end)
{
    char *headers = malloc(header_end + 1);
    if (!headers)
    {
        return 0;
    }

    memcpy(headers, raw, header_end);
    headers[header_end] = '\0';

    char *saveptr = NULL;
    char *line = strtok_r(headers, "\r\n", &saveptr);
    size_t length = 0;

    while ((line = strtok_r(NULL, "\r\n", &saveptr)) != NULL)
    {
        if (strncasecmp(line, "Content-Length:", 15) == 0)
        {
            char *value = line + 15;
            while (*value == ' ' || *value == '\t')
            {
                value++;
            }
            length = strtoul(value, NULL, 10);
            break;
        }
    }

    free(headers);
    return length;
}

int server_listen(int port)
{
    SOCKET server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == INVALID_SOCKET)
    {
        printf("socket() failed: %d\n", WSAGetLastError());
        return -1;
    }

    int opt = 1;

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(opt)) < 0)
    {
        printf("setsockopt() failed: %d\n", WSAGetLastError());
        closesocket(server_fd);
        return -1;
    }

    struct sockaddr_in address;

    memset(&address, 0, sizeof(address));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        closesocket(server_fd);
        return -1;
    }

    if (listen(server_fd, 128) < 0)
    {
        closesocket(server_fd);
        return -1;
    }

    return server_fd;
}

void connection_handle(int client_fd, Router *router)
{
    char *buffer = malloc(MAX_REQUEST_SIZE + 1);
    if (!buffer)
    {
        closesocket(client_fd);
        return;
    }

    size_t total = 0;
    int header_end = -1;
    size_t content_length = 0;

    while (total < MAX_REQUEST_SIZE)
    {
        ssize_t received = recv(client_fd, buffer + total, MAX_REQUEST_SIZE - total, 0);

        if (received <= 0)
        {
            break;
        }

        total += received;
        buffer[total] = '\0';

        if (header_end == -1)
        {
            header_end = find_header_end(buffer, total);

            if (header_end != -1)
            {
                content_length = extract_content_length(buffer, header_end);
            }
        }

        if (header_end != -1 && total >= (size_t)header_end + content_length)
        {
            break;
        }
    }

    Response response;
    response_init(&response);

    if (total == 0 || header_end == -1)
    {
        response_set_status(&response, 400);
        response_set_content_type(&response, "application/json");
        response_set_body(&response, "{\"error\":\"bad request\"}");
        response_send(client_fd, &response);
        response_free(&response);
        free(buffer);
        closesocket(client_fd);
        return;
    }

    Request request;

    if (request_parse(buffer, total, &request) != 0)
    {
        response_set_status(&response, 400);
        response_set_content_type(&response, "application/json");
        response_set_body(&response, "{\"error\":\"failed to parse request\"}");
        response_send(client_fd, &response);
        response_free(&response);
        free(buffer);
        closesocket(client_fd);
        return;
    }

    router_dispatch(router, &request, &response);

    response_send(client_fd, &response);

    request_free(&request);
    response_free(&response);
    free(buffer);
    closesocket(client_fd);
}
