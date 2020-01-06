#ifndef TFTP_LIB
#define TFTP_LIB

#include "tftp_consts.h"

/** Get packet type */
int tftp_get_type(char* buff, int len);

/** Send RRQ, the parameters are all used as input */
int tftp_send_rrq(int sd, char* file, char* mode, struct sockaddr_in addr);

/** Send file from specified filename, the parameters are all used as input */
int tftp_send_file(int sd, char* filename, char* mode, struct sockaddr_in* addr);

/** Send ACK, the parameters are all used as input */
int tftp_send_ack(int sd, uint16_t seq_n, struct sockaddr_in* addr);

/** Send error message, the parameters are all used as input */
int tftp_send_error(int sd, uint16_t error_code, char* error_msg, struct sockaddr_in* addr);

/** Unpack error, msg and error_code will be updated according to the unpacked message */
int tftp_unpack_error(char* pkt, int pkt_len, char* msg, int msg_len, uint16_t* error_code);

/** Unpack RRQ, file and mode will be updated according to the unpacked message */
int tftp_unpack_rrq(char* buf, int buf_len, char* file, int file_len, char* mode, int mode_len);

/** Unpack DATA, data and block_n will be updated according to the unpacked message */
int tftp_unpack_data(char* pkt, int pkt_size, char* data, int data_size, uint16_t* block_n);

/** Unpack ACK, seq_n will be updated according to the unpacked message */
int tftp_unpack_ack(char* pkt, int pkt_size, uint16_t* seq_n);

#endif