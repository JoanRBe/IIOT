#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <wiringPiI2C.h>
#include <time.h>

// Dirección I2C del AHT20
#define AHT20_I2C_ADDRESS 0x38

// Comandos del AHT20
#define CMD_INITIALIZE 0xBE
#define CMD_TRIGGER_MEASURE 0xAC
#define CMD_SOFT_RESET 0xBA

/**
 * @brief Inicializa el sensor AHT20.
 * 
 * @param fd Descriptor de archivo del dispositivo I2C.
 * @return int Devuelve 0 si la inicialización es exitosa, -1 en caso de error.
 */
 
int aht20_init(int fd) {
    // Enviar comando de inicialización
    if (wiringPiI2CWrite(fd, CMD_INITIALIZE) != 0) {
        perror("Error al inicializar el AHT20");
        return -1;
    }
    // Esperar a que el sensor esté listo
    usleep(150000); // 150 ms
    return 0;
}

/**
 * @brief Lee temperatura y humedad del AHT20.
 * 
 * @param fd: Descriptor de archivo del dispositivo I2C.
 * @param humidity: Variable para almacenar la humedad relativa.
 * @param temperature: Variable para almacenar la temperatura.
 * @return int: Devuelve 0 si la lectura es exitosa, -1 en caso de error.
 */
 
int aht20_read(int fd, float *humidity, float *temperature) {
    unsigned char cmd[3] = {CMD_TRIGGER_MEASURE, 0x33, 0x00};
    unsigned char data[6] = {0};

    // Enviar comando de medición
    if (write(fd, cmd, 3) != 3) {
        perror("Error al enviar comando de medición");
        return -1;
    }

    // Esperar a que los datos estén disponibles (85 ms)
    usleep(85000);

    // Leer los 6 bytes de datos
    if (read(fd, data, 6) != 6) {
        perror("Error al leer datos del AHT20");
        return -1;
    }

    // Verificar bit de estado
    if ((data[0] & 0x80) != 0) {
        fprintf(stderr, "El sensor no está listo\n");
        return -1;
    }

    // Calcular humedad (20 bits)
    unsigned int raw_humidity = ((data[1] << 12) | (data[2] << 4) | (data[3] >> 4));
    *humidity = (raw_humidity / 1048576.0) * 100.0;

    // Calcular temperatura (20 bits)
    unsigned int raw_temperature = ((data[3] & 0x0F) << 16) | (data[4] << 8) | data[5];
    *temperature = ((raw_temperature / 1048576.0) * 200.0) - 50.0;

    return 0;
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
    int fd;
    float temperature, humidity = 0;

    // Inicializar conexión I2C
    fd = wiringPiI2CSetup(AHT20_I2C_ADDRESS);
    if (fd < 0) {
        perror("Error al abrir el bus I2C");
        return -1;
    }

    // Inicializar el AHT20
    if (aht20_init(fd) < 0) {
        return -1;
    }

    // Leer y mostrar valores de temperatura y humedad
    while (1) {
        if (aht20_read(fd, &humidity, &temperature) == 0) {
            printf("Temperatura: %f ºC\nHumedad: %f %% \n", temperature, humidity);
        } else {
            printf("Error al leer datos del AHT20\n");
        }
        sleep(1); // Leer cada 2 segundos
    }

    return 0;
}


