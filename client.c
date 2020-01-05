#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "utils.h"
#include "tftp_lib.h"

#define CMDLINE_BUFFER_SIZE 100

char* tx_mode;

int print_help(){
    printf("Sono disponibili i seguenti comandi:\n");
    printf("!help --> mostra l'elenco dei comandi disponibili\n");
    printf("!mode {txt|bin} --> imposta il modo di trasferimento ");
    printf("dei file (testo o binario)\n");
    printf("!get filename nome_locale --> richiede al server il nome del file ");
    printf("<filename> e lo salva localmente con il nome <nome_locale>\n");
    printf("!quit --> termina il client\n");
}

int change_tx_mode(char* mode){
    if(mode == NULL){
        printf("Specificare un modo di trasferimento.\nDisponibili bin e txt\n");
    } else if(strcmp(mode, TX_BIN_MODE) == 0){
        tx_mode = TX_BIN_MODE;
        printf("Modo di trasferimento binario configurato\n");
    } else if(strcmp(mode, TX_TXT_MODE) == 0){
        tx_mode = TX_TXT_MODE;
        printf("Modo di trasferimento testuale configurato\n");
    } else {
        printf("Modo specificato sconosciuto.\nDisponibili bin e txt\n");
    }
}

void start_dl(char* sv_file, char* cl_file, char* sv_ip, int sv_port){
    struct sockaddr_in sv_addr;
    int sd, ret;

    //Inizializzazione

    //Scriviamo l'indirizzo del server
    memset(&sv_addr, 0, sizeof(sv_addr));
    sv_addr.sin_family = AF_INET;
    sv_addr.sin_port = htons(sv_port);
    inet_pton(AF_INET, sv_ip, &sv_addr.sin_addr);

    //Apriamo il socket UDP
    sd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sd < 0){
        perror("Errore nella creazione del socket: ");
    }

    printf("Richiesta file %s al server in corso.\n", sv_file);

    //Mandiamo l'RRQ per il file sv_file tramite il socket sd
    ret = tftp_send_rrq(sd, sv_file, tx_mode, sv_addr);
    if(ret < 0){
        //Errore gia' stampato dalla send_rrq
        return;
    }

    return;
}

int main(int argc, char** argv){
    int sv_port;
    char* sv_ip;
    char cmdline_buf[CMDLINE_BUFFER_SIZE];
    char* cmdline, *cmd_ptr;
    char* sv_file, *cl_file;
    int exit = 0;
    tx_mode = TX_BIN_MODE;

    //Impostiamo ip e porta del server
    if(argc < 3){
        printf("Parametri del server non specificati\n");
        sv_ip = DEFAULT_IP;
        sv_port = DEFAULT_PORT;
    } else {
        sv_ip = argv[1];
        sv_port = atoi(argv[2]);
    }
    printf("Server configurato con indirizzo %s:%d\n", sv_ip, sv_port);

    //Avviamo la modalita' interattiva
    while(!exit){
        printf("> ");
        if(fgets(cmdline_buf, CMDLINE_BUFFER_SIZE, stdin) == NULL){
            //EOF detected
            exit = 1;
        } else {
            //Splittiamo il comando al primo spazio
            cmdline = trim(cmdline_buf);
            cmd_ptr = strtok(cmdline, " ");
            //Otteniamo il comando da eseguire
            if(cmd_ptr == NULL){
                continue;
            } else if(strcmp(cmd_ptr, "!quit") == 0){
                exit = 1;
            } else if(strcmp(cmd_ptr, "!help") == 0){
                print_help();
            } else if(strcmp(cmd_ptr, "!mode") == 0){
                //Splittiamo il token un'altra volta
                cmd_ptr = strtok(NULL, " ");
                change_tx_mode(cmd_ptr);
            } else if(strcmp(cmd_ptr, "!get") == 0){
                //I due token successivi saranno i nomi dei file.
                sv_file = strtok(NULL, " ");
                if(sv_file == NULL){
                    printf("Specificare il nome del file da richiedere.\n");
                    continue;
                }

                cl_file = strtok(NULL, " ");
                if(cl_file == NULL){
                    printf("Specificare il nome del file da salvare.\n");
                    continue;
                }

                start_dl(sv_file, cl_file, sv_ip, sv_port);

            } else {
                printf("Comando non riconosciuto.\n");
                printf("Digita !help per la lista dei comandi disponibili.\n");
            }
        }
    }

    return 0;
}