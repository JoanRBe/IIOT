CC=gcc
LIBS= -lwiringPi -lm


all: sensors lm35 aht20

sensors: sensors.o 

	$(CC) -o sensors sensors.o $(LIBS)

sensors.o: sensors.c

	$(CC) -c sensors.c -o sensors.o

lm35: lm35.o 

	$(CC) -o lm35 lm35.o $(LIBS)

lm35.o: lm35.c

	$(CC) -c lm35.c -o lm35.o 

aht20: aht20.o 

	$(CC) -o aht20 aht20.o $(LIBS)

aht20.o: aht20.c

	$(CC) -c aht20.c -o aht20.o

clean:

	rm -f *.o sensors lm35 aht20