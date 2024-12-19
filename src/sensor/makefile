CC = arm-linux-gnueabihf-g++
LIBS = -lwiringPi -lm
LIBRARY = libhttp.so
SQLIB = libsql.so
CFLAGS = -Wall -fPIC
LDFLAGS = -shared
LDFLAGSQL = -L. -lsqlite3 -lstdc++
CFLAGSQL = -Wall -I/usr/include

all: sensors

# El objetivo principal
sensors: sensors.o $(LIBRARY) $(SQLIB)
	$(CC) -o sensors sensors.o $(LIBS) -L. -lhttp -L. -lsql -lsqlite3 -lstdc++

# Compilación de sensores.o
sensors.o: sensors.cpp
	$(CC) -c sensors.cpp -o sensors.o

# Compilación de la biblioteca de http
$(LIBRARY): http.o
	$(CC) $(LDFLAGS) -o $(LIBRARY) http.o

# Compilación de http.o
http.o: http.c
	$(CC) $(CFLAGS) -c http.c -o http.o

# Compilación de la biblioteca de SQL
$(SQLIB): sql.o
	$(CC) $(LDFLAGSQL) -o $(SQLIB) sql.o

# Compilación de sql.o
sql.o: sql.cpp
	$(CC) $(CFLAGS) $(CFLAGSQL) -c sql.cpp -o sql.o

# Comando de limpieza
clean:
	rm -f *.o sensors lm35 aht20 led voc-co2 sql report.txt

# Ejecutar la aplicación con un intervalo de 5 segundos
test:
	export LD_LIBRARY_PATH=.:$$LD_LIBRARY_PATH
	./sensors 2