CC=gcc
CFLAGS=-c -Wall

all: sql

	./sql
	doxygen Doxyfile

sql: sql.o

	arm-linux-gnueabihf-gcc sql.c -o sql -L. -lsqlite3 -I/usr/include

	g++ sql.c -l sqlite3

	touch report.txt

sql.o: sql.c

	$(CC) $(CFLAGS) sql.c

clean:

	rm a.out
	rm sql
	rm sql.o
	rm -rf html/
	rm -rf latex/
	rm report.txt