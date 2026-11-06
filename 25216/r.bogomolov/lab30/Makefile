CC = gcc
CFLAGS = -std=c99 -Wall -Wextra
LDLIBS = -lsocket -lnsl

all: client server

client: client.c
	$(CC) $(CFLAGS) client.c -o client $(LDLIBS)

server: server.c
	$(CC) $(CFLAGS) server.c -o server $(LDLIBS)

clean:
	rm -f client server

.PHONY: all clean