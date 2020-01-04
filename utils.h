#ifndef UTILS
#define UTILS

#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>

/**  Binds the socket to the specified port */
int bind_to_port(int socket, unsigned short port);

#endif