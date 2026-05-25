#include <string.h>
#include "router.h"

void router_init(Router *router)
{
    router->route_count = 0;
}

int router_add(Router *router, HttpMethod method, const char *path, RouteHandler handler)
{
    if (router->route_count >= MAX_ROUTES)
    {
        return -1;
    }

    Route *route = &router->routes[router->route_count];

    route->method = method;
    strncpy(route->path, path, MAX_PATH - 1);
    route->handler = handler;

    router->route_count++;
    return 0;
}

void router_dispatch(Router *router, Request *request, Response *response)
{
    for (int i = 0; i < router->route_count; i++)
    {
        Route *route = &router->routes[i];

        if (route->method == request->method && strcmp(route->path, request->path) == 0)
        {
            route->handler(request, response);
            return;
        }
    }

    response_set_status(response, 404);
    response_set_content_type(response, "application/json");
    response_set_body(response, "{\"error\":\"route not found\"}");
}
