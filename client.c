#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "utils.h"

#define BUFFER_SIZE 512
#define DEFAULT_IP "127.0.0.1"
#define DEFAULT_PORT 12345

int print_help(){
    printf("Sono disponibili i seguenti comandi:\n");
    printf("!help --> mostra l'elenco dei comandi disponibili\n");
    printf("!mode {txt|bin} --> imposta il modo di trasferimento ");
    printf("dei file (testo o binario)\n");
    printf("!get filename nome_locale --> richiede al server il nome del file ");
    printf("<filename> e lo salva localmente con il nome <nome_locale>\n");
    printf("!quit --> termina il client\n");
}

int main(int argc, char** argv){
    int ret, sd, sv_port;
    char* sv_ip;
    char buffer[] = "Ciao000";
    struct sockaddr_in sv_addr;

    //Impostiamo l'addr del server
    if(argc < 3){
        printf("Parametri del server non specificati\n");
        sv_ip = DEFAULT_IP;
        sv_port = DEFAULT_PORT;
    } else {
        sv_ip = argv[1];
        sv_port = atoi(argv[2]);
    }
    printf("Uso %s:%d\n", sv_ip, sv_port);
    memset(&sv_addr, 0, sizeof(sv_addr));
    sv_addr.sin_family = AF_INET;
    sv_addr.sin_port = htons(sv_port);
    inet_pton(AF_INET, sv_ip, &sv_addr.sin_addr);

    sd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sd > 0){
        printf("Socket creato con successo\n");
    } else {
        printf("Errore nella creazione del socket\n");
    }

    ret = sendto(sd, buffer, sizeof(buffer), 0,
            (struct sockaddr*)&sv_addr, sizeof(sv_addr));
    if(ret < 0){
        perror("Errore: ");
    } else {
        printf("%d", ret);
    }

    return 0;
}