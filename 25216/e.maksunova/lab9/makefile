DATA_FILE = file.txt

all:
	gcc -Wall -m32 -o prog32 lab3.c
	gcc -Wall -m64 -o lab3 lab3.c

run: all
	./lab3

run32: all
	./prog32

clean: 
	rm -f prog32 lab3