#ifndef ROUTER_H
#define ROUTER_H

#include "request.h"
#include "response.h"

#define MAX_ROUTES 128

typedef void (*RouteHandler)(Request *request, Response *response);

typedef struct
{
    HttpMethod method;
    char path[MAX_PATH];
    RouteHandler handler;
} Route;

typedef struct
{
    Route routes[MAX_ROUTES];
    int route_count;
} Router;

void router_init(Router *router);
int router_add(Router *router, HttpMethod method, const char *path, RouteHandler handler);
void router_dispatch(Router *router, Request *request, Response *response);

#endif
