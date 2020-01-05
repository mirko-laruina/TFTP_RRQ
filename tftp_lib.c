#include <stdlib.h>
#include <stdio.h>

#include "utils.h"
#include "tftp_lib.h"

int tftp_send_rrq(int sd, char* file, char* mode, struct sockaddr_in addr){
    unsigned int len, txlen;
    char* buffer, *buf_ptr;

    uint16_t opcode = RRQ_TYPE;    

    //Calcoliamo la lunghezza del messaggio
    len = sizeof(opcode) + strlen(file) + 2 + strlen(mode) + 2;
    buffer = malloc(len);
    if(buffer == NULL){
        //Un raro caso di malloc che fallisce
        printf("Errore nella creazione della richiesta.\n");
        return -1;
    }
    memset(buffer, 0, len);
    buf_ptr = buffer;

    //Costruiamo il messaggio
    //(prendo buffer, ne faccio il cast a uint16_t e lo dereferenzio per scrivere nei primi 2 byte)
    *((uint16_t*)buf_ptr) = htons(opcode);
    buf_ptr += 2;
    strcpy(buf_ptr, file);
    buf_ptr += strlen(file)+1;
    //Lasciamo il byte vuoto (la memset di prima ci assicura sia 0x00)
    buf_ptr += 1;
    strcpy(buf_ptr, mode);
    //L'ultimo byte gia' a 0x00

    //Inviamo il messaggio
    txlen = sendto(sd, buffer, len, 0, (struct sockaddr*)&addr, sizeof(addr));
    if(txlen != len){
        //Messaggio non inviato correttamente
        perror("Errore nell'invio della richiesta");
        return -1;
    }

    free(buffer);
    return 0;
}

int tftp_get_type(char* buff, int len){
    if(len > 2){
        return ntohs(*((uint16_t*)buff));
    }
    return -1;
}

int tftp_unpack_rrq(char* buf, int buf_len,
                    char* file, int file_len,
                    char* mode, int mode_len)
{
    char* curr_field = buf;
    int field_len;

    //Jump type field
    curr_field += 2;

    //Extract filename
    field_len = strlen(curr_field)+1;
    if(field_len > file_len || curr_field+field_len > buf+buf_len){
        logit("Il nome del file e' troppo lungo: %s\n", curr_field);
        return -1;
    }
    logit("File richiesto: %s\n", curr_field);
    strcpy(file, curr_field);
    curr_field += field_len+1;

    //Extract mode
    field_len = strlen(curr_field)+1;
    if(field_len > mode_len || curr_field+field_len > buf+buf_len){
        logit("Il modo specificato e' troppo lungo: %s\n", curr_field);
        return -1;
    }
    logit("Specificato modo: %s\n", curr_field);
    strcpy(mode, curr_field);

    curr_field += field_len+1;
    if(curr_field == buf+buf_len){
        logit("Pacchetto RRQ valido.\n");
    } else {
        logit("Pacchetto lungo %d invece di %d", curr_field-buf, buf_len);
        return -1;
    }
    return 0;
}

int tftp_send_data(int sd, char* data, int size, int seq_n, struct sockaddr_in* addr){
    char pkt[TFTP_MAX_DATA_PKT];
    char* pkt_ptr = pkt;
    int sent_bytes;

    *((uint16_t*)pkt_ptr) = htons(DATA_TYPE);
    pkt_ptr += 2;
    *((uint16_t*)pkt_ptr) = htons((uint16_t) seq_n);
    pkt_ptr += 2;
    memcpy(pkt_ptr, data, size);

    logit("Invio il pacchetto %d.\n", seq_n);
    sent_bytes = sendto(sd, pkt, size+TFTP_DATA_HEADER_SIZE, 0, (struct sockaddr*)addr, sizeof(*addr));

    if(sent_bytes != size+TFTP_DATA_HEADER_SIZE){
        printf("Here %d, %d", sent_bytes, size);
        return -1;
    }
    return 0;
}

int tftp_send_file(int sd, char* filename, char* moden, struct sockaddr_in* addr){
    FILE* fptr, *fptr_temp;
    char* tmpfile;
    char c, prevc, nextc;
    char block[TFTP_MAX_DATA_BLOCK];
    char ack_pkt[TFTP_ACK_SIZE];
    int eflag = 0;
    int bin, read_bytes, rcv_bytes, acked_n;
    int status;
    uint16_t block_n = 0;

    if(strcmp(moden, TX_TXT_MODE) == 0){
        bin = 0;
    } else if (strcmp(moden, TX_BIN_MODE) == 0){
        bin = 1;
    } else {
        logit("Modo usato sconosciuto.\n");
        return -1;
    }

    if(bin){
        fptr = fopen(filename, "rb");
    } else {
        //Come descritto qui https://stackoverflow.com/questions/7101068/tftp-protocol-implementation-and-difference-between-netascii-and-octect
        //Il file deve essere adattato CR->CR/NUL e LF->CR/LF 
        fptr_temp = fopen(filename, "r");
        // +5 = +1(dimensione totale array) + 5 (.temp)
        tmpfile = malloc(strlen(filename)+6);
        tmpfile = strdup(filename);
        strcat(tmpfile, ".temp");
        fptr = fopen(tmpfile, "w");
        if(fptr == NULL){
            logit("Impossibile creare il file temporaneo.\n");
            return -1;
        }

        //Iteriamo su tutto il file char a char
        prevc = '\0';
        while( !eflag && (c = (char) fgetc(fptr_temp)) != EOF){
            //CR -> \r -> \r\0
            if(c == '\r'){
                nextc = (char) fgetc(fptr_temp);
                if(nextc == '\0'){
                    if( putc('\r', fptr) == EOF || putc('\0', fptr) == EOF){
                        eflag = 1;
                    }
                } else {
                    ungetc(nextc, fptr_temp);
                }
            } else if(c == '\n' && prevc != '\r'){
                //LF -> CR/LF
                if (putc('\r', fptr) == EOF || putc('\n', fptr) == EOF){
                    eflag = 1;
                }
            } else {
                //Ricopiamo il carattere
                if (putc(c, fptr) == EOF){
                    eflag = 1;
                }
            }
            prevc = c;
        }

        if(eflag){
            logit("Errore nella scrittura del file temporaneo.\n");
        } else {
            logit("File convertito con successo in netascii.\n");
        }

        fclose(fptr);
        fclose(fptr_temp);
        fptr = fopen(tmpfile, "r");
        free(tmpfile);
    }

    if(fptr == NULL){
        logit("Impossibile aprire il file specificato.\n");
        return -1;
    }

    do {
        read_bytes = fread(block, 1, TFTP_MAX_DATA_BLOCK, fptr);
        if(tftp_send_data(sd, block, read_bytes, block_n, addr) < 0){
            logit("Errore nel trasferimento.\n");
            return -1;
        }
        
        rcv_bytes = recvfrom(sd, ack_pkt, TFTP_ACK_SIZE, 0, NULL, NULL);
        status = tftp_unpack_ack(ack_pkt, rcv_bytes, &acked_n);
        if(status < 0){
            return -1;
        }
        if(acked_n != block_n){
            logit("ACK ricevuto per %d, atteso per %d.\n", acked_n, block_n);
            return -1;
        }
        logit("ACK ricevuto per %d.\n", acked_n);
        block_n += 1;
    }
    while (read_bytes == TFTP_MAX_DATA_BLOCK);



    fclose(fptr);
    return 0;
}

int tftp_unpack_data(char* pkt, int pkt_size,
                     char* data, int data_size,
                     int* block_n)
{
    if(pkt_size < 4 || ntohs(*((uint16_t*)pkt)) != DATA_TYPE ){
        logit("Ricevuto un pacchetto non valido.\n");
        return -1;
    }
    char* ptr = pkt+2;
    *block_n = ntohs(*((uint16_t*)ptr));
    ptr += 2;

    if(pkt_size-4 > data_size){
        logit("Buffer insufficiente per il pacchetto ricevuto.\n");
        return -1;
    }
    memcpy(data, ptr, pkt_size-4);
    return pkt_size-4;
}

int tftp_send_ack(int sd, int seq_n, struct sockaddr_in* addr){
    int tx;
    char pkt[TFTP_ACK_SIZE];
    char* ptr = pkt;
    *((uint16_t*)ptr) = htons(TFTP_ACK_TYPE);
    ptr+=2;
    *((uint16_t*)ptr) = htons((uint16_t)seq_n);

    tx = sendto(sd, pkt, TFTP_ACK_SIZE, 0, (struct sockaddr*)addr, sizeof(*addr));
    if(tx != TFTP_ACK_SIZE){
        perror("Error");
        return -1;
    }
    return 0;
}

int tftp_unpack_ack(char* pkt, int pkt_size, int* seq_n){
    char* ptr = pkt;
    if(pkt_size < 4 || ntohs(*((uint16_t*)ptr)) != TFTP_ACK_TYPE ){
        logit("Ricevuto un pacchetto non valido.\n");
        return -1;
    }
    ptr += 2;
    *seq_n = ntohs(*((uint16_t*)ptr));
    return 0;
}

int tftp_send_error(int sd, int error_code,
                    char* error_msg,
                    struct sockaddr_in* addr)
{
    int len, txed;
    char* pkt, *ptr;

    len = 2 + 2 + strlen(error_msg) + 1 + 1;
    pkt = malloc(len);
    if(pkt < 0){
        return -1;
    }
    memset(pkt, 0, len);
    ptr = pkt;
    
    *((uint16_t*)ptr) = htons(TFTP_ERROR_TYPE);
    ptr += 2;
    *((uint16_t*)ptr) = (uint16_t) htons(error_code);
    ptr += 2;
    
    strcpy(ptr, error_msg);
    ptr += strlen(error_msg) + 1;

    txed = sendto(sd, pkt, len, 0, (struct sockaddr*)addr, sizeof(*addr));
    if(txed != len){
        return -1;
    }

    free(pkt);
    return 0;
}

int tftp_unpack_error(char* pkt, int pkt_len,
                      char* msg, int msg_len,
                      int* error_code)
{
    int rcv_msg_len;
    char* ptr = pkt;
    if(pkt_len < 4 || *((uint16_t*)pkt) != ntohs(TFTP_ERROR_TYPE)){
        logit("Ricevuto pacchetto non valido.\n");
        return -1;
    }

    ptr += 2;
    *error_code = ntohs(*((uint16_t*)ptr));
    ptr += 2;

    rcv_msg_len = strlen(ptr);
    if(rcv_msg_len > msg_len){
        logit("Messaggio ricevuto troppo lungo per essere salvato.\n");
        return -1;
    }
    strcpy(msg, ptr);
    return 0;
}