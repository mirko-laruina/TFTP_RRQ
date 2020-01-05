#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "utils.h"
#include "tftp_lib.h"

int main(int argc, char** argv){
    int sd, status, sv_port, addrlen, pktlen;
    int req_type, pid;
    char *path;
    char buf[TFTP_MAX_LENGTH];
    struct sockaddr_in cl_addr;
    char cl_ip[INET_ADDRSTRLEN];
    char filen[TFTP_MAX_FILE_LENGTH];
    char moden[TFTP_MODE_LENGTH];
    
    log_pid = 0;

    if(argc < 3){
        logit("Parametri non validi.\n");
        logit("%s <porta> <directory files>\n", argv[0]);
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
    logit("Leggo da: %s\n", path);

    sd = socket(AF_INET, SOCK_DGRAM, 0);
    status = bind_to_port(sd, sv_port);
    if (status >= 0){
        logit("Binding avvenuto con successo sulla porta %d\n", sv_port);
    } else {
        perror("Errore nel binding");
        return -1;
    }

    addrlen = sizeof(cl_addr);
    logit("Avvio del server completato.\n");

    while(1){
        //"Ascoltiamo" per nuovi pacchetti
        pktlen = recvfrom(sd, buf, TFTP_MAX_LENGTH, 0, (struct sockaddr*)&cl_addr, &addrlen);
        inet_ntop(AF_INET, &cl_addr, cl_ip, INET_ADDRSTRLEN);
        req_type = tftp_get_type(buf, pktlen);
        if (req_type == 0x01){
            log_pid = 0;
            logit("Richiesta RRQ ricevuta da %s:%d.\n", cl_ip, cl_addr.sin_port);
            //Gestione della richiesta: spawn processo figlio
            pid = fork();
            if (pid < 0){
                logit("Errore nella creazione di un processo figlio.\n");
                return -1;
            } else if (pid > 0){
                //Processo padre
                logit("Creazione del processo figlio %d.\n", pid);
            } else {
                log_pid = getpid();
                logit("Inizio gestione della richiesta.\n");
                //Estraimo il file (e modo) richiesto e controlliamo che esista
                status = tftp_unpack_rrq(buf, pktlen,
                                filen, TFTP_MAX_FILE_LENGTH,
                                moden, TFTP_MODE_LENGTH);
                if(status < 0){
                    logit("Pacchetto RRQ non valido.\n");
                    return -1;
                }
            }
        } else {
            logit("Richiesta TFTP sconosciuta.\n");
        }
    }

    free(path);
    return 0;
}