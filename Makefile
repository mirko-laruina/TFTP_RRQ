GCCOPT = -Wall -g

all: client server

utils.o: utils.c
	gcc $(GCCOPT) -c utils.c -o utils.o

tftp_lib.o: tftp_lib.c
	gcc $(GCCOPT)  -c tftp_lib.c -o tftp_lib.o

client: client.o utils.o tftp_lib.o
	gcc $(GCCOPT) client.o utils.o tftp_lib.o -o client

server: server.o utils.o tftp_lib.o
	gcc $(GCCOPT) server.o utils.o tftp_lib.o -o server

clean:
	rm *o client server