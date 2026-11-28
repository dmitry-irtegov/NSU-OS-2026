CC = cc
CFLAGS = -v

TARGET = Lab_3
SRC = Lab_3.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -m64 -o $(TARGET) $(SRC)
	chmod u+s $(TARGET)
	@echo "Собрана 64-битная версия Lab_3 с SUID"

$(TARGET)_32: $(SRC)
	$(CC) $(CFLAGS) -m32 -o $(TARGET)_32 $(SRC)
	chmod u+s $(TARGET)_32
	@echo "Собрана 32-битная версия Lab_3_32 с SUID"

build32: $(TARGET)_32

clean:
	rm -f $(TARGET) $(TARGET)_32
	@echo "Очищено"

.PHONY: all build32 clean
