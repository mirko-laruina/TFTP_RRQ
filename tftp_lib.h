#ifndef TFTP_LIB
#define TFTP_LIB

#define DEFAULT_IP "127.0.0.1"
#define DEFAULT_PORT 12345

#define TX_BIN_MODE "netascii"
#define TX_TXT_MODE "octet"
#define TFTP_MODE_LENGTH 9

#define RRQ_TYPE 0x01
#define WRQ_TYPE 0x02
#define DATA_TYPE 0x03

#define TFTP_MAX_LENGTH 1024
#define TFTP_MAX_FILE_LENGTH 1011

#define TFTP_MAX_DATA_PKT 516
#define TFTP_MAX_DATA_BLOCK 512
#define TFTP_DATA_HEADER_SIZE 4

#define TFTP_ACK_SIZE 4
#define TFTP_ACK_TYPE 4

#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>

/* Send RRQ package */
int tftp_send_rrq(int sd, char* file, char* mode, struct sockaddr_in addr);

int tftp_get_type(char* buff, int len);

/* Writes filename and mode, respecting the sizes of the buffer */
int tftp_unpack_rrq(char* buf, int buf_len, char* file, int file_len, char* mode, int mode_len);

int tftp_send_file(int sd, char* filename, char* mode, struct sockaddr_in* addr);

int tftp_send_ack(int sd, int seq_n, struct sockaddr_in* addr);

#endif