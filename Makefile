SRC = sources/main.c sources/read.c
.DEFAULT_GOAL := gcc
gcc: 
	gcc $(SRC) -o splash -Wall 
clang: 
	clang $(SRC) -o clang -Wall 
