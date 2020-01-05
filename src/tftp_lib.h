#ifndef TFTP_LIB
#define TFTP_LIB

#include "tftp_consts.h"

/** Get package type */
int tftp_get_type(char* buff, int len);

/* Send RRQ package */
int tftp_send_rrq(int sd, char* file, char* mode, struct sockaddr_in addr);

int tftp_send_file(int sd, char* filename, char* mode, struct sockaddr_in* addr);

int tftp_send_ack(int sd, int seq_n, struct sockaddr_in* addr);

int tftp_send_error(int sd, int error_code, char* error_msg, struct sockaddr_in* addr);

int tftp_unpack_error(char* pkt, int pkt_len, char* msg, int msg_len, int* error_code);

/* Writes filename and mode, respecting the sizes of the buffer */
int tftp_unpack_rrq(char* buf, int buf_len, char* file, int file_len, char* mode, int mode_len);

int tftp_unpack_data(char* pkt, int pkt_size, char* data, int data_size, int* block_n);

int tftp_unpack_ack(char* pkt, int pkt_size, int* seq_n);

#endif