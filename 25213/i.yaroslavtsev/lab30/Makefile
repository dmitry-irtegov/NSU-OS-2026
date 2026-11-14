CC = cc
CFLAGS = -v
LDLIBS = -lsocket -lnsl

all: server client

server: server.c
	$(CC) $(CFLAGS) server.c -o server $(LDLIBS)

client: client.c
	$(CC) $(CFLAGS) client.c -o client $(LDLIBS)

clean:
	rm -f server client

.PHONY: all clean
