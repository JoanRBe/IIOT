#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include "vocco2.h"
#include <string>
#include <iostream>

#define I2C_ADDRESS 0x70 // Dirección I2C del sensor
#define BUFFER_SIZE 7    // Tamaño de datos recibidos del sensor

// Declaración de la función calculate_crc
uint8_t calculate_crc(uint8_t *data, int length);

// Función de lectura de datos del sensor VOC/CO2
void VZ89TEReadData(int file, uint8_t request, uint8_t *data) {
    uint8_t buffer[6];
    uint8_t crc;

    // Calcular CRC para la transmisión
    crc = request;
    crc = (crc + (crc / 0x100)) & 0xFF; // Reducir a 8 bits
    crc = 0xFF - crc;

    // Preparar el buffer de comando
    buffer[0] = request;
    buffer[1] = 0; // Relleno según el protocolo
    buffer[2] = 0;
    buffer[3] = 0;
    buffer[4] = 0;
    buffer[5] = crc;

    // Enviar el comando al sensor
    if (write(file, buffer, 6) != 6) {
        perror("Error al enviar comando al sensor");
        exit(1);
    }

    usleep(2000); // Esperar 2 ms

    // Solicitar datos del sensor
    if (read(file, data, BUFFER_SIZE) != BUFFER_SIZE) {
        perror("Error al leer datos del sensor");
        exit(1);
    }

    // Verificar CRC de los datos recibidos
    crc = calculate_crc(data, 6);
    if (crc != data[6]) {
        fprintf(stderr, "Error: CRC inválido. Recibido: 0x%02X, Calculado: 0x%02X\n", data[6], crc);
    }
}

// Definición de la función calculate_crc
uint8_t calculate_crc(uint8_t *data, int length) {
    uint8_t crc = 0;
    for (int i = 0; i < length; i++) {
        crc += data[i];
    }
    crc = (crc + (crc / 0x100)) & 0xFF; // Reducir a 8 bits
    return 0xFF - crc;
}

// Definición de la función vocco2 para obtener los valores de VOC y CO2
void vocco2(std::string &VOC, std::string &CO2) {
    int file = open("/dev/i2c-1", O_RDWR);
    if (file < 0) {
        perror("Error al abrir el bus I2C");
        exit(1);
    }

    if (ioctl(file, I2C_SLAVE, I2C_ADDRESS) < 0) {
        perror("No se pudo conectar con el dispositivo");
        exit(1);
    }

    uint8_t data[BUFFER_SIZE];
    uint16_t VOCvalue, CO2value;

    // Leer el sensor VOC/CO2
    VZ89TEReadData(file, 0x0C, data);
    VOCvalue = (data[0] - 13) * (1000 / 229);
    CO2value = (data[1] - 13) * (1600 / 229) + 400;

    // Convertir los valores VOCvalue y CO2value a cadenas
    VOC = std::to_string(VOCvalue);  // Usar std::to_string para convertir VOCvalue a string
    CO2 = std::to_string(CO2value);  // Usar std::to_string para convertir CO2value a string

    printf("VOC: %s ppb\n", VOC.c_str());
    printf("CO2: %s ppm\n", CO2.c_str());

    close(file);
}
