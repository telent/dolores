default: tests

tests: fns.o tests.o
test: tests
	./tests

install:
