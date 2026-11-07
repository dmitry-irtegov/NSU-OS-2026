CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99 -D_POSIX_C_SOURCE=200809L

LDFLAGS = -lsocket -lnsl

all: server client

server: server.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

client: client.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
