all: main.c
	gcc main.c

run: all
	./a.out
