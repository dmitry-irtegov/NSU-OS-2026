CC   = gcc
BITS = 64

CFLAGS = -std=c99 -Wall -m$(BITS) -D_POSIX_C_SOURCE=200809L

PROG = ex_time
SRC  = ex_time.c

all: $(PROG)

$(PROG): $(SRC)
	$(CC) $(CFLAGS) -o $@ $(SRC)

clean:
	rm -f $(PROG) *.o core

.PHONY: all clean
