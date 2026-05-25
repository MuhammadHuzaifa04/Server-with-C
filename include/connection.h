#ifndef CONNECTION_H
#define CONNECTION_H

#include "router.h"

int server_listen(int port);
void connection_handle(int client_fd, Router *router);

#endif
