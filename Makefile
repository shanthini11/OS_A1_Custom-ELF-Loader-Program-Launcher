MAKEFLAGS += --no-builtin-rules
.SUFFIXES:

.PHONY: all clean loader launcher test

all: bin/lib_simpleloader.so bin/launch test/fib

loader:
	$(MAKE) -C loader

launcher:
	$(MAKE) -C launcher

test:
	$(MAKE) -C test

bin/lib_simpleloader.so: loader
	@mv loader/lib_simpleloader.so bin/

bin/launch: launcher
	# nothing to mv, launcher Makefile will put it in ../bin/

test/fib: test

clean:
	$(MAKE) -C loader clean
	$(MAKE) -C launcher clean
	$(MAKE) -C test clean
	rm -f bin/*
