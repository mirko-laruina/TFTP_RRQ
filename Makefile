all: client server

utils.o: utils.c
	gcc -Wall -c utils.c -o utils.o

tftp_lib.o: tftp_lib.c
	gcc -Wall -c tftp_lib.c -o tftp_lib.o

client: client.o utils.o tftp_lib.o
	gcc -Wall client.o utils.o tftp_lib.o -o client

server: server.o utils.o tftp_lib.o
	gcc -Wall server.o utils.o tftp_lib.o -o server
	
clean:
	rm *o client server tftp_lib