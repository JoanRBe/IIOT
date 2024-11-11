CC = gcc
CFLAGS = -Wall -fPIC
LDFLAGS = -shared
TARGET = main            
LIBRARY = libemail.so

all: $(TARGET)

$(TARGET): main.o $(LIBRARY)

	$(CC) -o $(TARGET) main.o -L. -lemail

main.o: main.c

	$(CC) $(CFLAGS) -c main.c -o main.o

$(LIBRARY): email.o

	$(CC) $(LDFLAGS) -o $(LIBRARY) email.o

email.o: email.c

	$(CC) $(CFLAGS) -c email.c -o email.o

clean:
	rm -f *.o $(TARGET) $(LIBRARY)

test:
	LD_LIBRARY_PATH=. ./main -s "172.20.0.21" -o 1585504@campus.euss.org -d 1585504@campus.euss.org -t hola -f /home/pi/Desktop/IIOT/libs/email/textemail.txt
