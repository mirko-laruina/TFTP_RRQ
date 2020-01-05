#include <stdlib.h>
#include <stdio.h>

#include "utils.h"
#include "tftp_lib.h"

int tftp_send_rrq(int sd, char* file, char* mode, struct sockaddr_in addr){
    unsigned int len, txlen;
    char* buffer, *buf_ptr;

    uint16_t opcode = RRQ_TYPE;    

    //Calcoliamo la lunghezza del messaggio
    len = sizeof(opcode) + strlen(file) + 1 + strlen(mode) + 1;
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
    buf_ptr += strlen(file);
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
    field_len = strlen(curr_field);
    if(field_len > file_len || curr_field+field_len > buf+buf_len){
        logit("Il nome del file e' troppo lungo: %s\n", curr_field);
        return -1;
    }
    logit("File richiesto: %s\n", curr_field);
    strcpy(file, curr_field);
    curr_field += field_len+1;

    //Extract mode
    field_len = strlen(curr_field);
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
