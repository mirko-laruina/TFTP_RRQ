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
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_ERROR_MSG_SIZE 256

/** Used by the logit function, set to 0 display [MAIN], otherwise [log_pid] */
int log_pid;

/** Used by set_error function, will contain the error msg */
char error_msg[MAX_ERROR_MSG_SIZE];

/** Binds the socket to the specified port */
int bind_to_port(int socket, unsigned short port);

/** Trim the string in place: no memory allocated or freed */
char* trim(char* s);

/** Utility for logging, printing a text before the messages
*   Used with the server to specify if it is the parent
*   or a child who is printing a message
*   Uses variable arguments, same syntax as printf()
*/
void logit(char*s, ...);

/** Utility for error management: sets an error_msg
*   Uses variable arguments, same syntax as printf()
*/
void set_error(char*s, ...);

/** Utility for error management: prints last error_msg */
void pr_err();

/** Take an input file and converts it to netascii */
int netascii(FILE* origin_fp, FILE* dest_fp);

#endif