.SUFFIXES:
.PHONY: all clean

CC = gcc
CFLAGS = -m32 -nostdlib -no-pie

all: fib

fib: fib.c
	$(CC) $(CFLAGS) -o fib fib.c

clean:
	rm -f fib


