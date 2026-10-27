CC = gcc
CFLAGS = -I./include

TARGET = app

SRCS = src/main.c
OBJS = build/main.o 

all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p build
	$(CC) -o $(TARGET) $(OBJS)

build/%.o: src/%.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf build $(TARGET)

.PHONY: all clean
