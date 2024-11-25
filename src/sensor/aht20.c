#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <wiringPiI2C.h>
#include <time.h>

// Dirección I2C del AHT20
#define AHT20_ADDRESS 0x38

// Comandos del AHT20
#define AHT20_CMD_INITIALIZE 0xBE
#define AHT20_CMD_TRIGGER 0xAC
#define AHT20_CMD_SOFT_RESET 0xBA

// Función para inicializar el sensor AHT20
void aht20_init(int fd) {
    wiringPiI2CWrite(fd, AHT20_CMD_SOFT_RESET);  // Reiniciar el sensor
    usleep(20000);  // Esperar 20 ms
    wiringPiI2CWrite(fd, AHT20_CMD_INITIALIZE);  // Inicializar
    usleep(300000);  // Esperar 300 ms para inicialización completa
}

// Función para leer los valores de temperatura y humedad del AHT20
int sensor_AHT20(float *temperatura, float *humedad) {
    uint8_t data[6];

    // Dirección del sensor
    int fd = wiringPiI2CSetup(AHT20_ADDRESS);
    if (fd == -1) {
        printf("Error: No se puede acceder al sensor AHT20.\n");
        return -1;
    }

    // Enviar comando para iniciar la medición
    wiringPiI2CWrite(fd, AHT20_CMD_TRIGGER);
    usleep(80000);  // Esperar 80 ms para la conversión

    // Leer 6 bytes de datos desde el AHT20
    for (int i = 0; i < 6; i++) {
        data[i] = wiringPiI2CRead(fd);
    }

    // Verificar si los datos están listos
    if ((data[0] & 0x80) != 0) {
        return -1;  // Error, los datos no están listos
    }

    // Calcular humedad (20 bits)
    uint32_t raw_humidity = ((data[1] << 16) | (data[2] << 8) | data[3]) >> 4;
    *humedad = (raw_humidity * 100.0) / 1048576.0;

    // Calcular temperatura (20 bits)
    uint32_t raw_temperature = ((data[3] & 0x0F) << 16) | (data[4] << 8) | data[5];
    *temperatura = (raw_temperature * 200.0) / 1048576.0 - 50.0;

    return 0;  // Éxito
}

int main(int argc, char *argv[]) {
    // Comprobar si el usuario ha introducido el intervalo en segundos
    if (argc != 2) {
        printf("Uso: %s <intervalo_en_segundos>\n", argv[0]);
        return 1;
    }

    // Convertir el argumento a número entero para el intervalo
    int intervalo = atoi(argv[1]);
    if (intervalo <= 0) {
        printf("Error: El intervalo debe ser un número positivo.\n");
        return 1;
    }

    // Variables para almacenar los valores leídos del sensor
    float temperatura = 0.0;
    float humedad = 0.0;

    // Bucle infinito para leer el sensor periódicamente
    while (1) {
        // Llamar a la función que lee el sensor
        if (sensor_AHT20(&temperatura, &humedad) == 0) {
            // Mostrar los valores leídos
            time_t now = time(NULL);
            struct tm *t = localtime(&now);
            printf("%02d:%02d:%02d - Temperatura: %.2f°C, Humedad: %.2f%%\n", 
                   t->tm_hour, t->tm_min, t->tm_sec, temperatura, humedad);
        } else {
            printf("Error al leer el sensor AHT20.\n");
        }

        // Esperar el intervalo antes de la siguiente lectura
        sleep(intervalo);
    }

    return 0;
}

