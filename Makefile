default: tests

tests.o: tests.c $(wildcard *.ino)

install:

test:
	./tests
