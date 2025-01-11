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
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/ssl3.h>

// @help: Compilar
//    * https: 'gcc -DVULL_HTTPS main_client_ssl.c -lssl -lcrypto -o https'
//    * http:  'gcc main_client_ssl.c -o http'

//#define VULL_HTTPS

#define REQUEST_MSG_SIZE (1024*256)
#define SERVER_PORT_NUM 80
//#define VULL_HTTPS
/*#ifdef VULL_HTTPS
#define SERVER_PORT_NUM 80
#endif*/
#define FAIL    -1

const char *server = "192.168.11.249";
/************************
*
*
* tcpClient
*
*
*/

void http(const char* server, char* url, char* resposta)
{
    struct sockaddr_in serverAddr;
    int sockAddrSize;
    int sFd;
    int result;
    char buffer[REQUEST_MSG_SIZE];
    char missatge[1024];

    // Imprimir la URL para depuración
    printf("Enviando la URL: %s\n", url);

    // Preparar la solicitud GET
    sprintf(missatge, "GET %s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n", url, server);

    // Crear el socket
    sFd = socket(AF_INET, SOCK_STREAM, 0);
    if (sFd < 0) {
        perror("Error creando el socket");
        return;
    }

    // Construir la dirección
    sockAddrSize = sizeof(struct sockaddr_in);
    bzero((char *)&serverAddr, sockAddrSize);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT_NUM);
    serverAddr.sin_addr.s_addr = inet_addr(server);

    // Conexión
    result = connect(sFd, (struct sockaddr *) &serverAddr, sockAddrSize);
    if (result < 0) {
        printf("Error en establecer la conexión: dirección %s, puerto %d\n", inet_ntoa(serverAddr.sin_addr), ntohs(serverAddr.sin_port));
        return;
    }
    printf("Conexión establecida con el servidor: dirección %s, puerto %d\n", inet_ntoa(serverAddr.sin_addr), ntohs(serverAddr.sin_port));

    // Inicializar SSL
    SSL_CTX *ctx = SSL_CTX_new(TLS_client_method());
    if (!ctx) {
        ERR_print_errors_fp(stderr);
        close(sFd);
        return;
    }

    // Crear una nueva conexión SSL
    SSL *ssl = SSL_new(ctx);
    if (!ssl) {
        ERR_print_errors_fp(stderr);
        SSL_CTX_free(ctx);
        close(sFd);
        return;
    }

    // Establecer la conexión SSL
    SSL_set_fd(ssl, sFd);
    result = SSL_connect(ssl);
    if (result != 1) {
        fprintf(stderr, "Error en la conexión SSL: %d\n", result);
        ERR_print_errors_fp(stderr);
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        close(sFd);
        return;
    }
    printf("Conexión SSL establecida\n");

    // Enviar mensaje
    result = SSL_write(ssl, missatge, strlen(missatge));
    if (result < 0) {
        perror("Error al enviar el mensaje SSL");
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        close(sFd);
        return;
    }
    printf("Mensaje enviado al servidor (bytes %d): {%s}\n", result, missatge);

    // Recibir respuesta
    memset(buffer, 0, REQUEST_MSG_SIZE);
    result = SSL_read(ssl, buffer, REQUEST_MSG_SIZE);
    if (result < 0) {
        perror("Error al recibir la respuesta SSL");
    } else {
        buffer[result] = '\0'; // Asegurarse de que la respuesta sea una cadena terminada en null
        printf("Mensaje recibido del servidor: %s\n", buffer);
        if (strstr(buffer, "200 OK") != NULL) {
            printf("El servidor ha procesado correctamente los datos.\n");
        } else {
            printf("Error al procesar los datos: %s\n", buffer);
        }
        if (resposta) {
            strcpy(resposta, buffer);
        }
    }

    // Cerrar la conexión SSL
    SSL_free(ssl);
    SSL_CTX_free(ctx);

    // Cerrar el socket
    close(sFd);
}
