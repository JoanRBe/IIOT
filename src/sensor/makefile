CC=gcc


all: sensors lm35 aht20

sensors: sensors.o 

	$(CC) sensors -o sensors.o

sensors.o: sensors.c

	$(CC) -c sensors.c -o sensors.o

lm35: lm35.o 

	$(CC) lm35 -o lm35.o

lm35.o: lm35.c

	$(CC) -c lm35.c -o lm35.o 

aht20: aht20.o 

	$(CC) aht20 -o aht20.o

aht20.o: aht20.c

	$(CC) -c aht20.c -o aht20.o

clean:

	rm -rf *o sensors
	rm -rf *o lm35
	rm -rf *o aht20