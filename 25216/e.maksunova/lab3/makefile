DATA_FILE = file.txt

all:
	@touch $(DATA_FILE)
	@chmod 600 $(DATA_FILE)
	chmod o+rx .

	gcc -Wall -m32 -o prog32 lab3.c
	gcc -Wall -m64 -o prog64 lab3.c

share:
	chmod u+s prog64

open: 
	chmod o+rx prog64
	chmod o+rx .

run: all open
	./prog64

runadmin: all open share
	./prog64

run32: all
	chmod u+s prog32
	./prog32

clean: 
	rm -f prog32 prog64 $(DATA_FILE)