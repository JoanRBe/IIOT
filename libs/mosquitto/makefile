# Variables
CC = arm-linux-gnueabihf-g++
CXXFLAGS = -std=c++17 -Wall -I/usr/local/include/mqtt/
 # Opciones de compilación para C++
LIBS = -lpaho-mqttpp3 -lpaho-mqtt3c
LDFLAGS = -shared
LIBRARY = libpaho-mqttpp3.so
LIBDIR = src/CMakeFiles/paho-mqttpp3-shared.dir
OBJ = mqtt.o# Definimos los archivos objeto

# Regla principal
mqtt: $(OBJ)
	$(CC) $(OBJ) -o mqtt $(LIBS)

# Regla para compilar mqtt.o
mqtt.o: mqtt.cpp
	$(CC) $(CXXFLAGS) -c mqtt.cpp -o mqtt.o

# Regla para limpiar los archivos generados
clean:
	rm -f $(OBJ) mqtt

# Regla para probar el ejecutable
test: mqtt
	./mqtt