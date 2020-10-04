.DEFAULT_GOAL := gcc
CFLAGS = -Wall -Iinclude

gcc: CC = gcc 
gcc: bin obj bin/splash

clang: CC = clang
clang: bin obj bin/splash

debug: CC = gcc
debug: CFLAGS += -pedantic -fsanitize=leak,address
debug: bin obj bin/splash

obj: bin/redirect.o bin/read.o bin/main.o

bin/redirect.o: sources/redirect.c
	$(CC) -c sources/redirect.c -o bin/redirect.o $(CFLAGS)

bin/read.o: sources/read.c
	$(CC) -c sources/read.c -o bin/read.o $(CFLAGS)

bin/main.o: sources/main.c
	$(CC) -Iinclude -c sources/main.c -o bin/main.o $(CFLAGS)

bin/splash: bin/main.o bin/redirect.o bin/read.o
	$(CC) bin/main.o bin/redirect.o bin/read.o $(CFLAGS) -o bin/splash

bin:
	mkdir -p bin

clean:
	rm -rf bin
