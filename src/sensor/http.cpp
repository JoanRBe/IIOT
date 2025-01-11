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

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define REQUEST_MSG_SIZE (1024*256)
#define SERVER_PORT_NUM 80  // Puerto 80 para HTTP
#define FAIL    -1

const char *server = "192.168.11.249";

/************************
*
* tcpClient
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

    // Enviar mensaje
    result = write(sFd, missatge, strlen(missatge));
    if (result < 0) {
        perror("Error al enviar el mensaje");
        close(sFd);
        return;
    }
    printf("Mensaje enviado al servidor (bytes %d): {%s}\n", result, missatge);

    // Recibir respuesta
    memset(buffer, 0, REQUEST_MSG_SIZE);
    result = read(sFd, buffer, REQUEST_MSG_SIZE);
    if (result < 0) {
        perror("Error al recibir la respuesta");
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

    // Cerrar el socket
    close(sFd);
}
