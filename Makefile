default: test

tests: fns.o tests.o
test: tests
	./tests

out/dolores.ino.bin: *.ino *.cpp *.h
	arduino --verbose-build --board esp8266:esp8266:d1 --pref update.check=false  --pref build.path=out/ --verify dolores.ino

install:
