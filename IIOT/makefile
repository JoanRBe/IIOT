
CC=gcc

all: client_smtp

help:
	@echo "Objectius possibles:\n"
	@echo "  * help   :Aquesta ajuda"
	@echo "  * all    :Compilar/construir executables"
	@echo "  * clean  :Netejar/borrar projecte"
	@echo "  * doc    :Crear documentacio"
	@echo ""

build/.deixarme:
	mkdir build
	touch build/.deixarme

build: build/.deixarme
	
client_smtp: build build/client_smtp

build/client_smtp: build src/client_smtp/main.c
	${CC} src/client_smtp/main.c -o build/client_smtp


clean:
	rm -rf build/
