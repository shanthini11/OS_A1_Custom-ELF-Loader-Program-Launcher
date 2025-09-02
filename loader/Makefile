CC = gcc
CFLAGS = -m32 -Wall -fPIC
LDFLAGS = -shared

all: lib_simpleloader.so

lib_simpleloader.so: loader.c loader.h
	$(CC) $(CFLAGS) -o $@ loader.c $(LDFLAGS)

clean:
	rm -f lib_simpleloader.so
