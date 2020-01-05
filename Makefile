GCC_OPT = -Wall -g

all: tftp_client tftp_server

utils.o: utils.c
	gcc $(GCC_OPT) -c utils.c -o utils.o

tftp_lib.o: tftp_lib.c
	gcc $(GCC_OPT)  -c tftp_lib.c -o tftp_lib.o

tftp_client: tftp_client.o utils.o tftp_lib.o
	gcc $(GCC_OPT) tftp_client.o utils.o tftp_lib.o -o tftp_client

tftp_server: tftp_server.o utils.o tftp_lib.o
	gcc $(GCC_OPT) tftp_server.o utils.o tftp_lib.o -o tftp_server

clean:
	rm *o tftp_client tftp_server