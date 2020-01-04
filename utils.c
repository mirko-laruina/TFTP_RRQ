#include "utils.h"

int bind_to_port(int socket, unsigned short port){
    struct sockaddr_in my_addr;
    int ret;

    //Allocazione struttura indirizzo di ascolto
    memset(&my_addr, 0, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(port);
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    //Creazione socket
    ret = bind(socket, (struct sockaddr*)&my_addr, sizeof(my_addr));
    return ret;
}

char* trim(char* str){
    int i;

    //Ignora gli spazi iniziali
    while(isspace(*str)){
        str++;
    }
    //Ignora gli spazi finali e inserisci il carattere di fine stringa
    i = strlen(str) - 1;
    while(isspace(str[i])){
        i--;
    }
    str[i+1] = '\0';
    return str;
}