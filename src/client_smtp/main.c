#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>


#define REQUEST_MSG_SIZE	1024
#define REPLY_MSG_SIZE		1024
#define SERVER_PORT_NUM		25



/**TCPIPClient**/

/**Parametrització en C**/

/**Introducció de parámetres**/

int main (int argc, char *argv[])
{

	char missatge[1024];
	char buffer[1024];
	char servidor[32];
	char usuari[64];
	char remitent[64];
	char destinatari[64];
	char assumpte[64];
	char text[256];
	int opt = 0;
	int check = 0;

	static struct option long_options[] = {
		{"servidor", required_argument, 0, 's'},
		{"usuari", required_argument, 0, 'u'},
		{"remitent", required_argument, 0,  'r'},
		{"destinatari", required_argument, 0, 'd'},
		{"assumpte", required_argument, 0, 'a'},
		{"text", required_argument, 0, 't'},
		{0, 0, 0, 0}
    };

	int long_index = 0;
	while ((opt = getopt_long(argc, argv, "s:u:r:d:a:t:", long_options, &long_index)) != -1) {
		switch (opt) {
		case 's':
		sprintf (servidor, optarg);
			break;
		case 'u':
		sprintf (usuari, optarg);
			break;
		case 'r':
		sprintf (remitent, optarg);
			break;
		case 'd':
		sprintf (destinatari, optarg);
			break;
		case 'a':
		sprintf (assumpte, optarg);
			break;
		case 't':
		sprintf (text, optarg);
		check = -1;
			break;
		default:
		printf("Getopt error1");;
			}}

    if (check == 0) {
		printf("Getopt error2");
		exit(EXIT_FAILURE);
    }

    else {
		
	FILE *file = fopen(text, "r");
		if (file == NULL) {
			perror("No s'ha pogut accedir a l'arxiu");
			exit(EXIT_FAILURE);}
		
		memset(text, 0, 256);
		fread(text, sizeof(char), 256, file);
		fclose(file);

	struct sockaddr_in	serverAddr;
	char serverName[32]; //"172.20.0.21" Adreça IP del servidor.
	int	sockAddrSize;
	int	sFd;
	int result;

	/*
	printf("Servidor: %s\n", servidor);
	printf("Usuari: %s\n", usuari);
	printf("Remitent: %s\n", remitent);
	printf("Destinatari: %s\n", destinatari);
	printf("Assumpte: %s\n", assumpte);
	*/
	printf("Text: %s\n", text);
	

	/*Crear el socket*/
	sFd = socket(AF_INET, SOCK_STREAM, 0);
	sprintf (serverName, servidor);

	/*Construir l'adreça*/
	sockAddrSize = sizeof(struct sockaddr_in);
	bzero ((char *)&serverAddr, sockAddrSize); /**Posar l'estructura a zero. @param servidor variable per entrar ip servidor*/
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons (SERVER_PORT_NUM);
	serverAddr.sin_addr.s_addr = inet_addr(serverName);

	/*Connexió*/
	result = connect (sFd, (struct sockaddr *) &serverAddr, sockAddrSize);

	if (result < 0) {
	printf("Error en establir la connexió\n");
	exit(-1);
	}

	else {
	printf("\nConnexió establerta amb el servidor: adreça %s, port %d\n", inet_ntoa(serverAddr.sin_addr), ntohs(serverAddr.sin_port));
	}

	/*Rebre*/
	result = read(sFd, buffer, 256);
	printf("Missatge rebut del servidor(bytes %d): %s\n", result, buffer); //Primer hem de llegir el buffer.

   /*Enviar*/
	sprintf(missatge, "HELO %s\n", usuari); // Ens identifiquem com a usuari.
	strcpy(buffer, missatge); //Copiar missatge a buffer.
	result = write(sFd, buffer, strlen(buffer));
	printf("Missatge enviat a servidor(bytes %d): %s\n", result, missatge);

	/*Rebre*/
	result = read(sFd, buffer, 256);
	printf("Missatge rebut del servidor(bytes %d): %s\n", result, buffer); //Llegim el buffer.

	/*Enviar*/
	sprintf(missatge, "MAIL FROM: %s\n", remitent); //Remitent del mail.
	strcpy(buffer, missatge); //Copiar missatge a buffer.
	result = write(sFd, buffer, strlen(buffer));
	printf("Missatge enviat a servidor(bytes %d): %s\n", result, missatge);

	/*Rebre*/
	result = read(sFd, buffer, 256);
	printf("Missatge rebut del servidor(bytes %d): %s\n", result, buffer); //Llegim el buffer.

	/*Enviar*/
	sprintf(missatge, "RCPT TO: %s\n", destinatari); //Destinatari del mail.
	strcpy(buffer, missatge); //Copiar missatge a buffer.
	result = write(sFd, buffer, strlen(buffer));
	printf("Missatge enviat a servidor(bytes %d): %s\n", result, missatge);

	/*Rebre*/
	result = read(sFd, buffer, 256);
	printf("Missatge rebut del servidor(bytes %d): %s\n", result, buffer); //Llegim el buffer.

	/*Enviar*/
	sprintf(missatge, "DATA\n"); //Obrim el missatge.
	strcpy(buffer, missatge); //Copiar missatge a buffer.
	result = write(sFd, buffer, strlen(buffer));
	printf("Missatge enviat a servidor(bytes %d): %s\n", result, missatge);

	/*Rebre*/
	result = read(sFd, buffer, 256);
	printf("Missatge rebut del servidor(bytes %d): %s\n", result, buffer); //Llegim el buffer.

	/*Enviar*/
	sprintf(missatge, "\nDe: %s\nPer: %s\nAssumpte: %s\nMissatge: %s\n.\n", remitent, destinatari, assumpte, text);
	strcpy(buffer, missatge); //Copiar missatge a buffer.
	result = write(sFd, buffer, strlen(buffer));
	printf("Missatge enviat a servidor(bytes %d): %s\n", result, missatge);

	/*Rebre*/
	result = read(sFd, buffer, 256);
	printf("Missatge rebut del servidor(bytes %d): %s\n", result, buffer); //Llegim el buffer.

	/*Enviar*/
	strcpy(missatge, "QUIT\n"); //Obrim el missatge.
	strcpy(buffer, missatge); //Copiar missatge a buffer.
	result = write(sFd, buffer, strlen(buffer));
	printf("Missatge enviat a servidor(bytes %d): %s\n", result, missatge);

	/*Rebre*/
	result = read(sFd, buffer, 256);
	printf("Missatge rebut del servidor(bytes %d): %s\n", result, buffer); //Llegim el buffer.

	/*Tancar el socket*/
	close(sFd);

	return 0;
	}}
