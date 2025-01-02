# Definición del compilador
CC = arm-linux-gnueabihf-g++
CFLAGS = -Wall -I/usr/include
LDFLAGS = -L. -lsqlite3 -lstdc++

# Archivo fuente y nombre del ejecutable
SRC = sql.cpp
OUT = sql

# Regla para compilar el programa
$(OUT): $(SRC)
	$(CC) $(SRC) -o $(OUT) $(CFLAGS) $(LDFLAGS)

#Regla para ejecución de programa

test: sql

	./sql -I/usr/include -L. -lsqlite3 -lstdc++
	doxygen Doxyfile

# Regla para limpiar los archivos generados
clean:

	rm sql
	rm report.txt
	rm -rf html/
	rm -rf latex/