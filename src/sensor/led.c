#include <wiringPi.h>
#include <stdio.h>
#include <unistd.h>

// Definir el pin GPIO donde está conectado el LED
#define LED_PIN 15 // GPIO 14 corresponde al pin físico no se, wiringPi usa 15 para GPIO 14

int main() {
    // Inicializar wiringPi
    if (wiringPiSetup() == -1) {
        printf("Error al inicializar wiringPi\n");
        return 1;
    }

    // Configurar el pin como salida
    pinMode(LED_PIN, OUTPUT);

    // Bucle para encender y apagar el LED
    while (1) {
        printf("Encendiendo el LED\n");
        digitalWrite(LED_PIN, HIGH); // Encender el LED
        sleep(1); // Esperar 1 segundo

        printf("Apagando el LED\n");
        digitalWrite(LED_PIN, LOW); // Apagar el LED
        sleep(1); // Esperar 1 segundo
    }

    return 0;
}
