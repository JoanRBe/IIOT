#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <wiringPiI2C.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <time.h>
#include <string.h>
#include "http.h"

// Definiciones para AHT20
#define AHT20_I2C_ADDRESS 0x38
#define CMD_INITIALIZE 0xBE
#define CMD_TRIGGER_MEASURE 0xAC

// Definiciones para VZ89TE
#define VZ89TE_I2C_ADDRESS 0x70
#define VZ89TE_BUFFER_SIZE 7

// Definiciones para LM35
#define SINGLE_ENDED_CH0 0
static char *cntdevice = "/dev/spidev0.0";


// Variables globales
const char *id_sensor = "0";
float val_sens;

// Función para calcular CRC (VZ89TE)
uint8_t calculate_crc(uint8_t *data, int length) {
    uint8_t crc = 0;
    for (int i = 0; i < length; i++) {
        crc += data[i];
    }
    crc = (crc + (crc / 0x100)) & 0xFF; // Reducir a 8 bits
    return 0xFF - crc;
}

// Leer datos del VZ89TE
void VZ89TEReadData(int file, uint8_t request, uint8_t *data) {
    uint8_t buffer[6];
    uint8_t crc;

    crc = request;
    crc = (crc + (crc / 0x100)) & 0xFF;
    crc = 0xFF - crc;

    buffer[0] = request;
    buffer[1] = buffer[2] = buffer[3] = buffer[4] = 0;
    buffer[5] = crc;

    if (write(file, buffer, 6) != 6) {
        perror("Error al enviar comando al VZ89TE");
        exit(1);
    }

    usleep(2000);
    if (read(file, data, VZ89TE_BUFFER_SIZE) != VZ89TE_BUFFER_SIZE) {
        perror("Error al leer datos del VZ89TE");
        exit(1);
    }

    if (calculate_crc(data, 6) != data[6]) {
        fprintf(stderr, "Error: CRC inválido para VZ89TE\n");
    }
}

// Funciones para AHT20 y LM35 se mantienen como en el código original

// Programa Principal
int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Uso: %s <intervalo_en_segundos>\n", argv[0]);
        return 1;
    }

    int intervalo = atoi(argv[1]);
    if (intervalo <= 0) {
        printf("Error: El intervalo debe ser un número positivo.\n");
        return 1;
    }

    // Configuración para AHT20
    int fd_aht20 = wiringPiI2CSetup(AHT20_I2C_ADDRESS);
    if (fd_aht20 < 0) {
        perror("Error al abrir el bus I2C para AHT20");
        return -1;
    }
    if (aht20_init(fd_aht20) < 0) {
        return -1;
    }

    // Configuración para VZ89TE
    int fd_vz89te = open("/dev/i2c-1", O_RDWR);
    if (fd_vz89te < 0 || ioctl(fd_vz89te, I2C_SLAVE, VZ89TE_I2C_ADDRESS) < 0) {
        perror("Error al abrir el bus I2C para VZ89TE");
        return -1;
    }

    // Configuración para LM35
    int lm35_value;
    float lm35_volts, lm35_temperature;
    float humidity;
    uint8_t vz89te_data[VZ89TE_BUFFER_SIZE];
    uint16_t VOCvalue, CO2value;

    while (1) {
        // Leer AHT20
        if (aht20_read(fd_aht20, &humidity) == 0) {
            printf("AHT20 -> Humedad: %.2f %%\n", humidity);
            char url[1000];
            sprintf(url, "http://iotlab.euss.cat/cloud/guardar_dades_adaptat.php?id_sensor=402&valor=%.2f", humidity);
            char resposta[100000];
            http("192.168.11.249", url, resposta);
        } else {
            printf("Error al leer datos del AHT20\n");
        }

        // Leer VZ89TE
        VZ89TEReadData(fd_vz89te, 0x0C, vz89te_data);
        VOCvalue = (vz89te_data[0] - 13) * (1000 / 229);
        CO2value = (vz89te_data[1] - 13) * (1600 / 229) + 400;
        printf("VZ89TE -> VOC: %d ppb, CO2: %d ppm\n", VOCvalue, CO2value);
        sprintf(url, "http://iotlab.euss.cat/cloud/guardar_dades_adaptat.php?id_sensor=405&valor=%.2f", VOCvalue);
        sprintf(url, "http://iotlab.euss.cat/cloud/guardar_dades_adaptat.php?id_sensor=404&valor=%.2f", CO2value);

        // Leer LM35
        if (spiadc_config_transfer(SINGLE_ENDED_CH0, &lm35_value) >= 0) {
            lm35_volts = 3.3 * lm35_value / 1023;
            lm35_temperature = lm35_volts * 1000 / 10;
            printf("LM35 -> Temperatura: %.2f ºC\n", lm35_temperature);
            char url[1000];
            sprintf(url, "http://iotlab.euss.cat/cloud/guardar_dades_adaptat.php?id_sensor=401&valor=%.2f", lm35_temperature);
            char resposta[100000];
            http("192.168.11.249", url, resposta);
        } else {
            printf("Error al leer datos del LM35\n");
        }

        sleep(intervalo);
    }

    close(fd_vz89te);
    close(fd_aht20);

    return 0;
}
