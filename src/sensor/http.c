/***************************************************************************
                          main.c  -  client
                             -------------------
    begin                : lun feb  4 16:00:04 CET 2002
    copyright            : (C) 2002 by A. Moreno
    email                : amoreno@euss.es
    copyright            : (C) 2023 by A.Fontquerni & S.Bernadas
    email                : afontquerni@euss.cat / sbernadas@euss.cat
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

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

// @help: Compilar 
//    * https: 'gcc -DVULL_HTTPS main_client_ssl.c -lssl -lcrypto -o https'
//    * http:  'gcc main_client_ssl.c -o http'

//#define VULL_HTTPS

#define REQUEST_MSG_SIZE	1024*256
#ifdef VULL_HTTPS
#define SERVER_PORT_NUM		443
#else
#define SERVER_PORT_NUM		80
#endif

#define FAIL    -1

#ifdef VULL_HTTPS
#include <openssl/ssl.h>
#include <openssl/err.h>


SSL * init_SSL(int fd, SSL_CTX *ctx)
{
    SSL *ssl;

    SSL_library_init();

	// -- initCTX
    SSL_METHOD *method;
    //SSL_CTX *ctx;
    OpenSSL_add_all_algorithms();  /* Load cryptos, et.al. */
    SSL_load_error_strings();   /* Bring in and register error messages */
    method = TLSv1_2_client_method();  /* Create new client-method instance */
    ctx = SSL_CTX_new(method);   /* Create new context */
    if ( ctx == NULL )
    {
        ERR_print_errors_fp(stderr);
        //abort();
        return NULL;
    }

    ssl = SSL_new(ctx);      /* create new SSL connection state */
    SSL_set_fd(ssl, fd);    /* attach the socket descriptor */
    if ( SSL_connect(ssl) == FAIL ) {  /* perform the connection */
        ERR_print_errors_fp(stderr);
        //abort();
        return NULL;
	}

	return ssl;
}
#endif

 /************************
*
*
* tcpClient
*
*
*/

int main(int argc, char *argv[]){
	struct sockaddr_in	serverAddr;
	char	    serverName[] = "192.168.11.249"; //Adreça IP on està iotlab.euss.cat
	int			sockAddrSize;
	int			sFd;
	int 		result;
	char		buffer[REQUEST_MSG_SIZE];
	char		missatge[1024]; //= "GET http://iotlab.euss.cat/cloud/guardar_dades_adaptat.php?id_sensor=%s&valor=%s&temps=\r\n\r\n HTTP/1.1\r\n\r\n";
	const char		*id_sensor= "401";
	const char		*valor= "21";
	
	sprintf(missatge, "GET http://iotlab.euss.cat/cloud/guardar_dades_adaptat.php?id_sensor=%s&valor=%s&temps=\r\n\r\n HTTP/1.1\r\n\r\n", id_sensor, valor);

	/*Crear el socket*/
	sFd=socket(AF_INET,SOCK_STREAM,0);

	/*Construir l'adreça*/
	sockAddrSize = sizeof(struct sockaddr_in);
	bzero ((char *)&serverAddr, sockAddrSize); //Posar l'estructura a zero
	serverAddr.sin_family=AF_INET;
	serverAddr.sin_port=htons (SERVER_PORT_NUM);
	serverAddr.sin_addr.s_addr = inet_addr(serverName);

	/*Conexió*/
	result = connect (sFd, (struct sockaddr *) &serverAddr, sockAddrSize);
	if (result < 0)
	{
		printf("Error en establir la connexió: adreça %s, port %d\n",	inet_ntoa(serverAddr.sin_addr), ntohs(serverAddr.sin_port));
		exit(-1);
	}
	printf("\nConnexió establerta amb el servidor: adreça %s, port %d\n",	inet_ntoa(serverAddr.sin_addr), ntohs(serverAddr.sin_port));

#ifdef VULL_HTTPS
    SSL_CTX *ctx = NULL;
	SSL *ssl = init_SSL(sFd, ctx);
	if(ssl) {
		printf("'Secure Sockets Layer (SSL)' activada\n");
	}
	else {
		printf("ERROR en crear'Secure Sockets Layer (SSL)' !!!\n");
		abort();
		exit(-1);
	}
#endif

	/*Enviar*/
	memset(buffer, 0, REQUEST_MSG_SIZE);
	sprintf(buffer,missatge,id_sensor,valor); //Copiar missatge a buffer
#ifdef VULL_HTTPS
	result = SSL_write(ssl, buffer, strlen(buffer));
#else
	result = write(sFd, buffer, strlen(buffer));
#endif
	printf("Missatge enviat a servidor(bytes %d): {%s}\n",	result, buffer);

	/*Rebre*/
	memset(buffer, 0, REQUEST_MSG_SIZE);
#ifdef VULL_HTTPS
	result = SSL_read(ssl, buffer, REQUEST_MSG_SIZE);
#else
	result = read(sFd, buffer, REQUEST_MSG_SIZE);
#endif
	printf("Missatge rebut del servidor(bytes %d): {%s}\n",	result, buffer);

#ifdef VULL_HTTPS
	memset(buffer, 0, REQUEST_MSG_SIZE);
	result = SSL_read(ssl, buffer, REQUEST_MSG_SIZE);
	printf("Missatge2 rebut del servidor(bytes %d): {%s}\n",	result, buffer);
    SSL_free(ssl);        /* release connection state */
    SSL_CTX_free(ctx);        /* release context */
#endif

	/*Tancar el socket*/
	close(sFd);

	return 0;
	}
