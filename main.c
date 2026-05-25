#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include "connection.h"
#include "router.h"

void home_handler(Request *request, Response *response)
{
    response_set_content_type(response, "application/json");
    response_set_body(response, "{\"message\":\"C HTTP server is running\",\"status\":\"ok\"}");
}

void get_users_handler(Request *request, Response *response)
{
    response_set_content_type(response, "application/json");
    response_set_body(response, "{\"method\":\"GET\",\"route\":\"/users\",\"users\":[{\"id\":1,\"name\":\"Alice\"},{\"id\":2,\"name\":\"Bob\"}]}");
}

void post_users_handler(Request *request, Response *response)
{
    response_set_status(response, 201);
    response_set_content_type(response, "application/json");

    char body[2048];

    snprintf(
        body,
        sizeof(body),
        "{\"method\":\"POST\",\"route\":\"/users\",\"status\":\"created\",\"received\":\"%s\"}",
        request->body ? request->body : "");

    response_set_body(response, body);
}

void put_users_handler(Request *request, Response *response)
{
    response_set_content_type(response, "application/json");

    char body[2048];

    snprintf(
        body,
        sizeof(body),
        "{\"method\":\"PUT\",\"route\":\"/users\",\"status\":\"updated\",\"received\":\"%s\"}",
        request->body ? request->body : "");

    response_set_body(response, body);
}

void patch_users_handler(Request *request, Response *response)
{
    response_set_content_type(response, "application/json");

    char body[2048];

    snprintf(
        body,
        sizeof(body),
        "{\"method\":\"PATCH\",\"route\":\"/users\",\"status\":\"partially updated\",\"received\":\"%s\"}",
        request->body ? request->body : "");

    response_set_body(response, body);
}

void delete_users_handler(Request *request, Response *response)
{
    response_set_content_type(response, "application/json");
    response_set_body(response, "{\"method\":\"DELETE\",\"route\":\"/users\",\"status\":\"deleted\",\"message\":\"user deleted successfully\"}");
}

int main()
{
    printf("Initializing Windows Sockets...\n");
    WSADATA wsa_data;
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0)
    {
        printf("WSAStartup failed with error: %d\n", WSAGetLastError());
        return 1;
    }
    printf("WSAStartup successful\n");

    Router router;
    router_init(&router);

    router_add(&router, HTTP_GET, "/", home_handler);
    router_add(&router, HTTP_GET, "/users", get_users_handler);
    router_add(&router, HTTP_POST, "/users", post_users_handler);
    router_add(&router, HTTP_PUT, "/users", put_users_handler);
    router_add(&router, HTTP_PATCH, "/users", patch_users_handler);
    router_add(&router, HTTP_DELETE, "/users", delete_users_handler);

    int port = 8080;
    int server_fd = server_listen(port);

    if (server_fd < 0)
    {
        perror("server_listen");
        return 1;
    }

    printf("===========================================\n");
    printf("C HTTP Server Started\n");
    printf("===========================================\n");
    printf("Listening on http://localhost:%d\n", port);
    printf("Routes:\n");
    printf("  GET  /\n");
    printf("  GET  /users\n");
    printf("  POST /users\n");
    printf("  PUT  /users\n");
    printf("  PATCH /users\n");
    printf("  DELETE /users\n");
    printf("===========================================\n");

    while (1)
    {
        struct sockaddr_in client_address;
        int client_length = sizeof(client_address);

        int client_fd = accept(server_fd, (struct sockaddr *)&client_address, &client_length);

        if (client_fd < 0)
        {
            perror("accept");
            continue;
        }

        connection_handle(client_fd, &router);
    }

    closesocket(server_fd);
    WSACleanup();
    return 0;
}
