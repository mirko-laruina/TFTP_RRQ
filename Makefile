all: client server

utils.o: utils.c
	gcc -Wall -c utils.c -o utils.o

client: client.o utils.o
	gcc -Wall client.o utils.o -o client

server: server.o utils.o
	gcc -Wall server.o utils.o -o server

lib: tftp_lib.o
	gcc -Wall tftp_lib.o -o tftp_lib

clean:
	rm *o client server tftp_lib