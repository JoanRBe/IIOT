CC = arm-linux-gnueabihf-g++
LIBS = -lwiringPi -lm -lssl -lcrypto -lsqlite3 -lstdc++ -lpaho-mqttpp3 -lpaho-mqtt3c -lpthread
LIBRARY = libhttp.so
MQTTLIB = libpaho-mqttpp3.so
SQLIB = libsql.so

CFLAGS = -Wall -fPIC
HTTPS_FLAGS = -DVULL_HTTPS -lssl -lcrypto
LDFLAGS = -shared
LDFLAGSQL = -L. -lsqlite3 -lstdc++
CFLAGSQL = -Wall -I/usr/include
CXXFLAGS = -std=c++17 -Wall -I/usr/local/include/mqtt/

# Archivos fuente
OBJ = mqtt.o vocco2.o http.o sql.o

# Directorios de las bibliotecas
LIBDIR = .

# Objetivo principal
sensors: sensors.o $(OBJ) $(LIBRARY) $(SQLIB)
	$(CC) $(HTTPS_FLAGS) -o sensors sensors.o $(OBJ) $(LIBS) -L. -lhttp -L. -lsql

# Compilación de sensors.o
sensors.o: sensors.cpp mqtt.o vocco2.o http.o sql.o
	$(CC) $(HTTPS_FLAGS) $(CXXFLAGS) -c sensors.cpp -o sensors.o

# Compilación de la biblioteca HTTP
$(LIBRARY): http.o
	$(CC) -shared -fPIC -o $(LIBRARY) http.o -lssl -lcrypto

# Compilación de http.o
http.o: http.cpp
	$(CC) -fPIC -c http.cpp -o http.o

# Compilación de la biblioteca SQL
$(SQLIB): sql.o
	$(CC) $(LDFLAGSQL) -shared -fPIC -o $(SQLIB) sql.o

# Compilación de sql.o
sql.o: sql.cpp
	$(CC) -fPIC -c sql.cpp -o sql.o

# Regla para compilar mqtt
mqtt: $(OBJ)
	$(CC) $(OBJ) -o mqtt $(LIBS)

# Regla para compilar mqtt.o
mqtt.o: mqtt.cpp
	$(CC) $(CXXFLAGS) -c mqtt.cpp -o mqtt.o

# Comando de limpieza
clean:
	rm -f *.o sensors mqtt http sql $(LIBRARY) $(SQLIB)

# Ejecutar la aplicación con un intervalo de 2 segundos
test:
	export LD_LIBRARY_PATH=.:$$LD_LIBRARY_PATH
	./sensors 2