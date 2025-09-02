.SUFFIXES:
.PHONY: all clean

CC = gcc
CFLAGS = -m32 -Wall -I../loader
LDFLAGS = -L../bin -l_simpleloader

all: ../bin/launch

../bin/launch: launch.c
	$(CC) $(CFLAGS) -o $@ launch.c $(LDFLAGS) -Wl,-rpath,'$$ORIGIN'

clean:
	rm -f ../bin/launch
