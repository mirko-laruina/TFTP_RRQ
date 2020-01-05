#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "utils.h"
#include "tftp_lib.h"

int main(int argc, char** argv){
    int sd, status, sv_port, addrlen, pktlen;
    int req_type;
    char *path;
    char buf[MAX_LENGTH];
    struct sockaddr_in cl_addr;

    if(argc < 3){
        printf("Parametri non validi.\n");
        printf("%s <porta> <directory files>\n", argv[0]);
        return -1;
    }
    sv_port = atoi(argv[1]);

    //Controlliamo che la cartella esista e prendiamo il path canonico
    //Specificando NULL, realpath alloca la stringa e la ritorna
    path = realpath(argv[2], NULL);
    if(path == NULL){
        perror("Errore nel percorso");
        return -1;
    }
    printf("Leggo da: %s\n", path);

    sd = socket(AF_INET, SOCK_DGRAM, 0);
    status = bind_to_port(sd, sv_port);
    if (status >= 0){
        printf("Binding avvenuto con successo sulla porta %d\n", sv_port);
    } else {
        perror("Errore nel binding");
        return -1;
    }

    addrlen = sizeof(cl_addr);
    printf("Avvio del server completato.\n");

    while(1){
        //"Ascoltiamo" per nuovi pacchetti
        pktlen = recvfrom(sd, buf, MAX_LENGTH, 0, (struct sockaddr*)&cl_addr, &addrlen);
        req_type = tftp_get_type(buf, pktlen);
    }

    free(path);
    return 0;
}