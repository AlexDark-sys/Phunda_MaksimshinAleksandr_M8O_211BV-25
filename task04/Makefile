CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -Wpedantic -O2
LDLIBS = -lm
TARGET = program

all: $(TARGET)
$(TARGET): main.o functions.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)
main.o: main.c head.h
	$(CC) $(CFLAGS) -c main.c
functions.o: functions.c head.h
	$(CC) $(CFLAGS) -c functions.c
clean:
	rm -f $(TARGET) *.o
.PHONY: all clean
