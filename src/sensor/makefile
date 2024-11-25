CC=gcc


all: sensors

sensors: sensors.o 

	$(CC) -o sensors sensors.o

sensors.o: sensors.c

	$(CC) -c -o sensors.o sensors.c

lm35: lm35.o 

	$(CC) -o lm35 lm35.o

lm35.o: lm35.c

	$(CC) -c -o lm35.o lm35.c

aht20: aht20.o 

	$(CC) -o aht20 aht20.o

aht20.o: aht20.c

	$(CC) -c -o aht20.o aht20.c

clean:

	rm -rf *o sensors
	rm -rf *o lm35
	rm -rf *o aht20