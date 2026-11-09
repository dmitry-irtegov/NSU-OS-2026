all: server client

server: server.c
	gcc -Wall -o server server.c -lsocket -lnsl

client: client.c
	gcc -Wall -o client client.c -lsocket -lnsl
