#include <libgen.h>
#include "utils.h"
#include "tftp_lib.h"

int start_ul(struct sockaddr_in* cl_addr, char* filename, char* moden){
    int sd, status;

    sd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sd < 0){
        logit("Errore nella creazione del socket.\n");
        return -1;
    }
    //Port 0 binds to a random port
    status = bind_to_port(sd, 0);
    if(status < 0){
        logit("Errore nel binding.\n");
        return -1;
    }
    
    status = tftp_send_file(sd, filename, moden, cl_addr);
    if(status < 0){
        return -1;
    }
    close(sd);
    return 0;
}

int main(int argc, char** argv){
    int sd, status, sv_port, pktlen;
    unsigned int addrlen;
    int req_type, pid;
    char *path;
    char buf[TFTP_MAX_LENGTH];
    struct sockaddr_in cl_addr;
    char cl_ip[INET_ADDRSTRLEN];
    char filen[TFTP_MAX_FILE_LENGTH];
    char moden[TFTP_MODE_LENGTH];
    char* realfile, *dir;
    
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
    chdir(path);
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
                realfile = realpath(filen, NULL);
                if(realfile == NULL){
                    logit("File locale non esistente: %s\n", filen);
                    tftp_send_error(sd, TFTP_NOT_FOUND, "File non trovato.\n", &cl_addr);
                    return -1;
                }
                dir = strdup(realfile);
                dir = dirname(dir);
                if(strlen(dir) < strlen(path)
                    || strncmp(dir, path, strlen(path)) != 0
                    || dir[strlen(path)] != '\0'){
                    logit("!!!Tentativo di intrusione!!!\n");
                    logit("Accesso negato al file richiesto: %s\n", realfile);
                    logit("Percorso consentito: %s\n", path);
                    tftp_send_error(sd, TFTP_ACCESS_VIOLATION, "Accesso negato.\n", &cl_addr);
                    return -1;
                }

                logit("File locale: %s\n", realfile);

                if(strcmp(moden, TFTP_TX_TXT_MODE) != 0 && strcmp(moden, TFTP_TX_BIN_MODE) != 0){
                    logit("Modo specificato non supportato: %s", moden);
                    return -1;
                }

                start_ul(&cl_addr, realfile, moden);

                fflush(stdout);
                free(realfile);
                free(dir);
            }
        } else {
            logit("Richiesta TFTP sconosciuta: %d.\n", req_type);
            tftp_send_error(sd, TFTP_ILLEGAL_OP, "Operazione non permessa.\n", &cl_addr);
        }
    }

    free(path);
    close(sd);
    return 0;
}