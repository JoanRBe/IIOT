CC=gcc
CFLAGS=-c -Wall

all: client_smtp

	$(CC) client_smtp.c -o client_smtp

client_smtp: client_smtp.o

	$(CC) client_smtp.o -o client_smtp

client_smtp.o: client_smtp.c

	$(CC) $(CFLAGS) client_smtp.c

clean:

	rm -rf *o client_smtp