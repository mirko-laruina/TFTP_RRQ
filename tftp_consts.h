#ifndef TFTP_CONSTS
#define TFTP_CONSTS

/** Defaults */
#define TFTP_DEFAULT_IP "127.0.0.1"
#define TFTP_DEFAULT_PORT 12345

/** Transfer modes */
#define TFTP_TX_BIN_MODE "octet"
#define TFTP_TX_TXT_MODE "netascii"
#define TFTP_MODE_LENGTH 9

/** Opcodes */
#define TFTP_RRQ_TYPE 0x01
#define TFTP_WRQ_TYPE 0x02
#define TFTP_DATA_TYPE 0x03
#define TFTP_ACK_TYPE 0x04
#define TFTP_ERROR_TYPE 0x05

/** Sizes */
#define TFTP_MAX_LENGTH 1024
#define TFTP_MAX_FILE_LENGTH 1011

#define TFTP_MAX_DATA_PKT 516
#define TFTP_MAX_DATA_BLOCK 512
#define TFTP_DATA_HEADER_SIZE 4

#define TFTP_ACK_SIZE 4

#define TFTP_MAX_ERR_MSG_LENGTH 256

/** Error codes */
#define TFTP_NOT_FOUND 0x01
#define TFTP_ILLEGAL_OP 0x04
#define TFTP_ACCESS_VIOLATION 0x02

#endif