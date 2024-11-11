#include <stdio.h>
#include "email.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctype.h>
#include <getopt.h>

#define REQUEST_MSG_SIZE 1024
#define REPLY_MSG_SIZE 500
#define SERVER_PORT_NUM 25


void email(char *server_address, char *email_destinatari, char *email_remitent, char *tema_email, char *text_email)
{
    struct sockaddr_in serverAddr;
    char BufRead[REPLY_MSG_SIZE];
    char BufWrite[REPLY_MSG_SIZE];
    int sockAddrSize, sFd, result;

    // Crear el socket
    sFd = socket(AF_INET, SOCK_STREAM, 0);
    if (sFd < 0) {
    perror("socket");
    exit(1);
    }

    // Construir la dirección
    sockAddrSize = sizeof(struct sockaddr_in);
    bzero((char *)&serverAddr, sockAddrSize);  // Poner la estructura a cero
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT_NUM);
    serverAddr.sin_addr.s_addr = inet_addr(server_address);

    // Conexión
    result = connect(sFd, (struct sockaddr *)&serverAddr, sockAddrSize);
    if (result < 0) {
    perror("connect");
    exit(1);
    }

    // Presentación servidor
    memset(BufRead, 0, REPLY_MSG_SIZE);
    result = read(sFd, BufRead, REPLY_MSG_SIZE);
    if (result < 0) {
    perror("read presentación");
    exit(1);
    }
    printf("Rebut (%d): -> %s\n", result, BufRead);

    // HELO
    memset(BufWrite, 0, REPLY_MSG_SIZE);
    sprintf(BufWrite, "HELO yourdomain.com\n");
    result = write(sFd, BufWrite, strlen(BufWrite));
    if (result < 0) {
    perror("write");
    exit(1);
    }

    memset(BufRead, 0, REPLY_MSG_SIZE);
    result = read(sFd, BufRead, REPLY_MSG_SIZE);
    printf("Rebut (%d): -> %s\n", result, BufRead);

    // MAIL FROM
    memset(BufWrite, 0, REPLY_MSG_SIZE);
    sprintf(BufWrite, "MAIL FROM: <%s>\n", email_remitent);
    result = write(sFd, BufWrite, strlen(BufWrite));

    memset(BufRead, 0, REPLY_MSG_SIZE);
    result = read(sFd, BufRead, REPLY_MSG_SIZE);
    printf("Rebut (%d): -> %s\n", result, BufRead);

    // RCPT TO
    memset(BufWrite, 0, REPLY_MSG_SIZE);
    sprintf(BufWrite, "RCPT TO: <%s>\n", email_destinatari);
    result = write(sFd, BufWrite, strlen(BufWrite));

    memset(BufRead, 0, REPLY_MSG_SIZE);
    result = read(sFd, BufRead, REPLY_MSG_SIZE);
    printf("Rebut (%d): -> %s\n", result, BufRead);

    // DATA
    memset(BufWrite, 0, REPLY_MSG_SIZE);
    sprintf(BufWrite, "DATA\n");
    result = write(sFd, BufWrite, strlen(BufWrite));

    memset(BufRead, 0, REPLY_MSG_SIZE);
    result = read(sFd, BufRead, REPLY_MSG_SIZE);
    printf("Rebut (%d): -> %s\n", result, BufRead);

    // Mensaje (correo)
    memset(BufWrite, 0, REPLY_MSG_SIZE);
    sprintf(BufWrite, "Subject: %s\nFrom: <%s>\nTo: <%s>\n\n%s\n.\n", tema_email, email_remitent, email_destinatari, text_email);
    result = write(sFd, BufWrite, strlen(BufWrite));

    memset(BufRead, 0, REPLY_MSG_SIZE);
    result = read(sFd, BufRead, REPLY_MSG_SIZE);
    printf("Rebut (%d): -> %s\n", result, BufRead);

    // QUIT
    memset(BufWrite, 0, REPLY_MSG_SIZE);
    sprintf(BufWrite, "QUIT\n");
    result = write(sFd, BufWrite, strlen(BufWrite));

    memset(BufRead, 0, REPLY_MSG_SIZE);
    result = read(sFd, BufRead, REPLY_MSG_SIZE);
    printf("Rebut (%d): -> %s\n", result, BufRead);

    // Cerrar el socket
    close(sFd);
}

