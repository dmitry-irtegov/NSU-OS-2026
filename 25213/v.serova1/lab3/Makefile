CC = cc

CFLAGS = -v -m64

TARGET = Lab_3
SRC = Lab_3.c
DATA_FILE = file.txt

all: suid

suid: $(TARGET) $(DATA_FILE)
	chmod u+s $(TARGET)
	@echo "SUID установлен на $(TARGET)"

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

$(DATA_FILE):
	touch $(DATA_FILE)
	chmod 600 $(DATA_FILE)

build32: $(SRC)
	$(CC) -v -m32 -o $(TARGET)_32 $(SRC)
	@echo "Собрана 32-битная версия $(TARGET)_32"

clean:
	rm -f $(TARGET) $(TARGET)_32 $(DATA_FILE)
	@echo "Очищено"

.PHONY: all suid build32 clean
