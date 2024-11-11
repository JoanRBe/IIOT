#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

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
#include "email.h"


#define REQUEST_MSG_SIZE 1024
#define REPLY_MSG_SIZE 500
#define SERVER_PORT_NUM 25

// Enviament e-mail (SMTP)

void print_error()
{
    printf("Error: Introdueix correctament server, origen, destí, tema i fitxer.\n");
    printf("--origen EMAIL\n");
    printf("--desti EMAIL\n");
    printf("--tema TEXT\n");
}

int main(int argc, char *argv[])
{
    int opt = 0;
    //int server = -1, origen = -1, desti = -1, tema =-1, fitxer =-1;
    // Pidiendo datos de la linia de comandes
    char server[20], desti[100], origen[100], tema[100], fitxer[100];;

    memset(server, 0, 20);
    memset(desti, 0, 100);
    memset(origen, 0, 100);
    memset(tema, 0, 100);
    memset(fitxer, 0, 100);

    //Specifying the expected options
    //The two options l and b expect numbers as argument
    static struct option long_options[] = {
    {"server",      required_argument,       0,  's' },
    {"origen", 		required_argument,       0,  'o' },
    {"desti",    	required_argument,       0,  'd' },
    {"tema",  		required_argument,       0,  't' },
    {"fitxer",      required_argument,       0,  'f' },
    {0,           0,                 0,  0   }
    };

    int long_index = 0;
    while ((opt = getopt_long(argc, argv, "s:o:d:t:f:", long_options, &long_index)) != -1) {
    switch (opt) {
    case 's':
    strcpy(server, optarg);
    break;
    case 'o':
    strcpy(origen, optarg);
    break;
    case 'd':
    strcpy(desti, optarg);
    break;
    case 't':
    strcpy(tema, optarg);
    break;
    case 'f':
    strcpy(fitxer, optarg);
    break;
    default:
    print_error("error");
    exit(EXIT_FAILURE);
    }
}
 // char serverName[] = "172.20.0.21";

//    scanf("%s", origen);
    printf("Server: %s\n", server);

//    scanf("%s", origen);
    printf("Origen: %s\n", origen);

 //   scanf("%s", desti);
    printf("Desti: %s\n", desti);

 //   scanf("%s", tema);
    printf("Tema: %s\n", tema);

   // printf("Message:\n");
  //  scanf(" %[^\n]s", message);  // Leer mensaje completo incluyendo espacios
    //printf("You wrote message: %s\n", message);

    // Leer el contenido del archivo especificado por --fitxer
    FILE *file = fopen(fitxer, "r");
    if (file == NULL) {
    perror("No es pot obrir el fitxer");
    exit(EXIT_FAILURE);
    }
    fread(fitxer, sizeof(char), 500, file);
    fclose(file);

    printf("Server: %s\n", server);
    printf("Origen: %s\n", origen);
    printf("Desti: %s\n", desti);
    printf("Tema: %s\n", tema);
    printf("Fitxer: %s\n", fitxer);

    // Llamar a la función para enviar el email
    email(server, desti, origen, tema, fitxer);

    return 0;

}
