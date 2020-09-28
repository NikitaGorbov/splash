SRC = sources/main.c sources/read.c sources/redirect.c
.DEFAULT_GOAL := gcc
gcc: 
	gcc $(SRC) -o splash -Wall
clang: 
	clang $(SRC) -o clang -Wall
debug:
	gcc $(SRC) -o splash -Wall -pedantic -fsanitize=leak,address
