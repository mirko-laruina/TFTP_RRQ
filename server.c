#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "utils.h"


int main(){
    
    int ret, sd, len;
    unsigned short port = 12345;
    char buf[512];
    struct sockaddr_in cl_addr;
    socklen_t addrlen = sizeof(cl_addr);

    sd = socket(AF_INET, SOCK_DGRAM, 0);
    ret = bind_to_port(sd, port);
    if (ret >= 0){
        printf("Binding avvenuto con successo\n");
    } else {
        perror("Errore nel binding: ");
        return -1;
    }

    ret = recvfrom(sd, buf, 512, 0, (struct sockaddr*)&cl_addr, &addrlen);
    if (ret>=0){
        printf("Messaggio ricevuto: ");
        printf(buf);
    } else {
        perror("Errore nella ricezione: ");
    }
    return 0;
}