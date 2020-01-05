#ifndef UTILS
#define UTILS

#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>

int log_pid;

/* Binds the socket to the specified port */
int bind_to_port(int socket, unsigned short port);

/* Trim the string in place (moving the ptr and '\0') */
char* trim(char* s);

void logit(char*s, ...);

#endif