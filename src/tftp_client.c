#include "utils.h"
#include "tftp_lib.h"

#define CMDLINE_BUFFER_SIZE 100

char* tx_mode;

void print_help(){
    printf("Sono disponibili i seguenti comandi:\n");
    printf("!help --> mostra l'elenco dei comandi disponibili\n");
    printf("!mode {txt|bin} --> imposta il modo di trasferimento ");
    printf("dei file (testo o binario)\n");
    printf("!get filename nome_locale --> richiede al server il nome del file ");
    printf("<filename> e lo salva localmente con il nome <nome_locale>\n");
    printf("!quit --> termina il client\n");
}

void change_tx_mode(char* mode){
    if(mode == NULL){
        printf("Specificare un modo di trasferimento.\nDisponibili bin e txt\n");
    } else if(strcmp(mode, "bin") == 0){
        tx_mode = TFTP_TX_BIN_MODE;
        printf("Modo di trasferimento binario configurato\n");
    } else if(strcmp(mode, "txt") == 0){
        tx_mode = TFTP_TX_TXT_MODE;
        printf("Modo di trasferimento testuale configurato\n");
    } else {
        printf("Modo specificato sconosciuto.\nDisponibili bin e txt\n");
    }
}

void handle_error(char* pkt, int pkt_length){
    char msg[TFTP_MAX_ERR_MSG_LENGTH];
    uint16_t ecode;

    if(!tftp_unpack_error(pkt, pkt_length, msg, TFTP_MAX_ERR_MSG_LENGTH, &ecode)){
        printf(msg);
    } else {
        pr_err();
    }
}

int start_dl(char* sv_file, char* cl_file, char* sv_ip, int sv_port){
    struct sockaddr_in sv_addr, csv_addr;
    unsigned int csv_addrlen;
    char buffer[TFTP_MAX_DATA_PKT];
    char data[TFTP_MAX_DATA_PKT];
    int sd, ret, read_bytes, pkt_type, wr_bytes, total_blocks;
    uint16_t exp_block_n, block_n;
    int tx_started = 0;
    char exists_choice;
    char cmdline_buf[CMDLINE_BUFFER_SIZE];
    FILE* fptr;

    //Inizializzazione

    //Scriviamo l'indirizzo del server
    memset(&sv_addr, 0, sizeof(sv_addr));
    sv_addr.sin_family = AF_INET;
    sv_addr.sin_port = htons(sv_port);
    inet_pton(AF_INET, sv_ip, &sv_addr.sin_addr);

    //Apriamo il socket UDP
    sd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sd < 0){
        perror("Errore nella creazione del socket");
    }

    ret = bind_to_port(sd, 0);
    if(ret < 0){
        perror("Errore nel binding");
        return -1;
    }

    //Creiamo il file da salvare
    if(access(cl_file, F_OK) != -1){
        printf("Un file dallo stesso nome esiste in locale. Sovrascriverlo? S/n\n");
        do {
            printf("> ");
            if(fgets(cmdline_buf, CMDLINE_BUFFER_SIZE, stdin) == NULL){
                close(sd);
                return -1;
            }
            exists_choice = trim(cmdline_buf)[0];
            if(exists_choice == 'S' || exists_choice == 's'){
                remove(cl_file);
                break;
            } else if(exists_choice == 'N' || exists_choice == 'n') {
                printf("Trasferimento annulato.\n");
                close(sd);
                return -1;
            }
        } while (printf("Scelta non valida. Si o no.\n"));
    }
    if(strcmp(tx_mode, TFTP_TX_TXT_MODE) == 0){
        fptr = fopen(cl_file, "w");
    } else if(strcmp(tx_mode, TFTP_TX_BIN_MODE) == 0){
        fptr = fopen(cl_file, "wb");
    }
    if(fptr < 0){
        printf("Errore nell'apertura del file.\n");
    }

    printf("Richiesta file %s al server in corso.\n", sv_file);

    //Mandiamo l'RRQ per il file sv_file tramite il socket sd
    ret = tftp_send_rrq(sd, sv_file, tx_mode, sv_addr);
    if(ret < 0){
        pr_err();
        close(sd);
        fclose(fptr);
        return -1;
    }

    exp_block_n = 0;
    total_blocks = 0;
    do{
        csv_addrlen = sizeof(csv_addr);
        ret = recvfrom(sd, buffer, TFTP_MAX_DATA_PKT, 0, (struct sockaddr*)&csv_addr, &csv_addrlen);
        pkt_type = tftp_get_type(buffer, ret);

        if (pkt_type == TFTP_DATA_TYPE){
            if(!tx_started){
                printf("Trasferimento in corso\n");
                tx_started = 1;
            }
            read_bytes = tftp_unpack_data(buffer, ret, data, TFTP_MAX_DATA_PKT, &block_n);
            if(read_bytes < 0){
                pr_err();
                close(sd);
                fclose(fptr);
                return -1;
            }

            if(exp_block_n != block_n){
                printf("Errore nel trasferimento: attesso il blocco %d, ricevuto %d.\n", exp_block_n, block_n);
                close(sd);
                fclose(fptr);
                return -1;
            }

            wr_bytes = fwrite(data, 1, read_bytes, fptr);
            if(wr_bytes != read_bytes){
                printf("Errore nel salvataggio del file.\n");
                close(sd);
                fclose(fptr);
                return -1;
            }

            ret = tftp_send_ack(sd, block_n, &csv_addr);

            if(ret < 0){
                printf("Errore nell'invio dell'ACK per il pacchetto %d.\n", block_n);
                close(sd);
                fclose(fptr);
                return -1;
            }
            exp_block_n += 1;
            total_blocks += 1;
        } else if (pkt_type == TFTP_ERROR_TYPE){
            handle_error(buffer, ret);
            close(sd);
            fclose(fptr);
            return -1;
        } else {
            printf("Ricevuto un pacchetto di tipo non atteso: ignoro.\n");
        }
    }
    while (read_bytes == TFTP_MAX_DATA_BLOCK);

    printf("Trasferimento completato (%d/%d blocchi).\n", total_blocks, total_blocks);
    printf("Salvataggio %s completato.\n", cl_file);

    close(sd);
    fclose(fptr);
    return 0;
}

int main(int argc, char** argv){
    int sv_port;
    char* sv_ip;
    char cmdline_buf[CMDLINE_BUFFER_SIZE];
    char* cmdline, *cmd_ptr;
    char* sv_file, *cl_file;
    int exit = 0;
    tx_mode = TFTP_TX_BIN_MODE;

    //Impostiamo ip e porta del server
    if(argc < 3){
        printf("Parametri del server non specificati\n");
        sv_ip = TFTP_DEFAULT_IP;
        sv_port = TFTP_DEFAULT_PORT;
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