all: serv clnt

serv: serv.c
	cc -v serv.c -o serv -lsocket

clnt: clnt.c
	cc -v clnt.c -o clnt -lsocket

clean:
	rm -f serv clnt

.PHONY: all clean
