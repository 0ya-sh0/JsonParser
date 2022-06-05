CC = gcc
CFLAGS = -g -Wall -Werror


demo: demo.o parser.o
	$(CC) $(CFLAGS) ./obj/demo.o ./obj/parser.o -o ./bin/demo

test: test.o parser.o
	$(CC) $(CFLAGS) ./obj/test.o ./obj/parser.o -o ./bin/test

parser.o: parser.c parser.h
	$(CC) -c $(CFLAGS) parser.c -o ./obj/parser.o

test.o: test.c
	$(CC) -c $(CFLAGS) test.c -o ./obj/test.o

demo.o: demo.c
	$(CC) -c $(CFLAGS) demo.c -o ./obj/demo.o

clean: 
	rm -rf ./obj ./bin
	mkdir obj bin

