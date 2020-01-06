#include "utils.h"

void logit(char* s, ... ){

    if(log_pid == 0){
        printf("[MAIN] ");
    } else {
        printf("[%d] ", log_pid);
    }

    va_list args;
    va_start(args, s);
    vprintf(s, args);
    va_end(args);
}

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

void set_error(char* s, ...){
    va_list args;
    va_start(args, s);
    vsnprintf(error_msg, MAX_ERROR_MSG_SIZE, s, args);
    va_end(args);
}

void pr_err(){
    printf("%s", error_msg);
}

int netascii(FILE* origin_fp, FILE* dest_fp){
    int eflag;
    char c, nextc, prevc;

    prevc = '\0';
    eflag = 0;
    while( !eflag && (c = (char) fgetc(origin_fp)) != EOF){
        //CR -> \r -> \r\0
        if(c == '\r'){
            nextc = (char) fgetc(origin_fp);
            if(nextc == '\0'){
                if( putc('\r', dest_fp) == EOF || putc('\0', dest_fp) == EOF){
                    eflag = 1;
                }
            } else {
                ungetc(nextc, origin_fp);
            }
        } else if(c == '\n' && prevc != '\r'){
            //LF -> CR/LF
            if (putc('\r', dest_fp) == EOF || putc('\n', dest_fp) == EOF){
                eflag = 1;
            }
        } else {
            //Ricopiamo il carattere
            if (putc(c, dest_fp) == EOF){
                eflag = 1;
            }
        }
        prevc = c;
    }

    if(eflag){
        return -1;
    }
    return 0;
}