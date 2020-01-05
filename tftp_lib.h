#ifndef TFTP_LIB
#define TFTP_LIB

#define DEFAULT_IP "127.0.0.1"
#define DEFAULT_PORT 12345

#define TX_BIN_MODE "bin"
#define TX_TXT_MODE "txt"

#define RRQ_TYPE 0x01
#define WRQ_TYPE 0x02

#define MAX_LENGTH 1024

#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>

/* Send RRQ package */
int tftp_send_rrq(int sd, char* file, char* mode, struct sockaddr_in addr);
int tftp_get_type(char* buff, int len);

#endif