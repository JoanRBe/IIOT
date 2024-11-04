CC=gcc
CFLAGS=-c -Wall

all: sql

	./sql

sql: sql.o

	arm-linux-gnueabihf-gcc sql.c -o sql -L. -lsqlite3 -I/usr/include

	g++ sql.c -l sqlite3

	touch informe.txt

sql.o: sql.c

	$(CC) $(CFLAGS) sql.c

clean:

	rm a.out
	rm sql
	rm sql.o
	rm informe.txt