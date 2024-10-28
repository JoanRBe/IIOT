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


#define REQUEST_MSG_SIZE	1024
#define REPLY_MSG_SIZE		1024
#define SERVER_PORT_NUM		25



 /**TCPIPClient**/
 
/**Parametrització en C**/

int main(int argc, char *argv[]){
	struct sockaddr_in	serverAddr;
	char serverName[32]; //"172.20.0.21" Adreça IP del servidor.
	int	sockAddrSize;
	int	sFd;
	int result;
	char missatge[1024];
	char buffer[1024];
	
int email(char *arg1, char *arg2, char *arg3, char *arg4);	
	char servidor[32];
	char usuari[64];
	char remitent[64];
	char destinatari[64];
	char assumpte[64];
	char text[256];

	//int mlen;
	
	/**Introducció de parámetres**/
	
	strcpy (servidor, "172.20.0.21");
	strcpy (serverName, servidor);
	strcpy (usuari, "1457962");
	strcpy (remitent, "1457962@campus.euss.org");
	strcpy (destinatari, "1457962@campus.euss.org");
	strcpy (assumpte, "Sprint 1");
	strcpy (text, "Hola, aquest és el mail parametritzat en C++ per entregar en Sprint 1");
	
	/*Crear el socket*/
	sFd=socket(AF_INET,SOCK_STREAM,0);

	/*Construir l'adreça*/
	sockAddrSize = sizeof(struct sockaddr_in);
	bzero ((char *)&serverAddr, sockAddrSize); //Posar l'estructura a zero.
	serverAddr.sin_family=AF_INET;
	serverAddr.sin_port=htons (SERVER_PORT_NUM);
	serverAddr.sin_addr.s_addr = inet_addr(serverName);

	/*Connexió*/
	result = connect (sFd, (struct sockaddr *) &serverAddr, sockAddrSize);
	
	if (result < 0){
	printf("Error en establir la connexió\n");
	exit(-1);}
	printf("\nConnexió establerta amb el servidor: adreça %s, port %d\n", inet_ntoa(serverAddr.sin_addr), ntohs(serverAddr.sin_port));

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
	}
