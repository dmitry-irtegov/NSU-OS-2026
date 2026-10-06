CC = cc
CFLAGS = -v

TARGET = shell
SRC = shell.c promptline.c parseline.c

$(TARGET): $(SRC) shell.h
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)

