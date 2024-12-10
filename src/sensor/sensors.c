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
#include "http.h"

// ----------------------------------------
// Configuración para AHT20
// ----------------------------------------

#define AHT20_I2C_ADDRESS 0x38
#define CMD_INITIALIZE 0xBE
#define CMD_TRIGGER_MEASURE 0xAC
#define CMD_SOFT_RESET 0xBA

int aht20_init(int fd) {
    if (wiringPiI2CWrite(fd, CMD_INITIALIZE) != 0) {
        perror("Error al inicializar el AHT20");
        return -1;
    }
    usleep(150000); // Esperar 150 ms
    return 0;
}

int aht20_read(int fd, float *humidity, float *temperature) {
    unsigned char cmd[3] = {CMD_TRIGGER_MEASURE, 0x33, 0x00};
    unsigned char data[6] = {0};

    if (write(fd, cmd, 3) != 3) {
        perror("Error al enviar comando de medición");
        return -1;
    }
    usleep(85000); // Esperar 85 ms

    if (read(fd, data, 6) != 6) {
        perror("Error al leer datos del AHT20");
        return -1;
    }

    if ((data[0] & 0x80) != 0) {
        fprintf(stderr, "El sensor no está listo\n");
        return -1;
    }

    unsigned int raw_humidity = ((data[1] << 12) | (data[2] << 4) | (data[3] >> 4));
    *humidity = (raw_humidity / 1048576.0) * 100.0;

    unsigned int raw_temperature = ((data[3] & 0x0F) << 16) | (data[4] << 8) | data[5];
    *temperature = ((raw_temperature / 1048576.0) * 200.0) - 50.0;

    return 0;
}

// ----------------------------------------
// Configuración para LM35
// ----------------------------------------

#define SINGLE_ENDED_CH0 0
static char *cntdevice = "/dev/spidev0.0";

static void pabort(const char *s) {
    perror(s);
    abort();
}

static void spiadc_config_tx(int conf, uint8_t tx[2]) {
    tx[0] = 0x01; 
    tx[1] = (conf & 0x07);
}

static int spiadc_transfer(int fd, uint8_t bits, uint32_t speed, uint16_t delay, uint8_t tx[2], uint8_t *rx, int len) {
    int ret, value;
    struct spi_ioc_transfer tr = {
        .tx_buf = (unsigned long)tx,
        .rx_buf = (unsigned long)rx,
        .len = len * sizeof(uint8_t),
        .delay_usecs = delay,
        .speed_hz = speed,
        .bits_per_word = bits,
    };

    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
    if (ret < 1)
        pabort("No se puede enviar el mensaje SPI");

    value = ((rx[0] & 0x1F) << 8) | rx[1];
    return value;
}

static int spiadc_config_transfer(int conf, int *value) {
    int fd, ret;
    uint8_t rx[2];
    uint8_t tx[2];
    uint8_t mode = SPI_MODE_0;
    uint8_t bits = 8;
    uint32_t speed = 500000;
    uint16_t delay = 0;

    fd = open(cntdevice, O_RDWR);
    if (fd < 0) {
        pabort("No se puede abrir el dispositivo SPI");
    }

    if (ioctl(fd, SPI_IOC_WR_MODE, &mode) == -1 || ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits) == -1 || ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed) == -1) {
        pabort("Error al configurar SPI");
    }

    spiadc_config_tx(conf, tx);
    ret = spiadc_transfer(fd, bits, speed, delay, tx, rx, 2);
    close(fd);

    *value = (rx[0] << 6) | (rx[1] >> 2);
    return ret;
}

// ----------------------------------------
// Programa Principal
// ----------------------------------------

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

    // Configuración de AHT20
    int fd_aht20 = wiringPiI2CSetup(AHT20_I2C_ADDRESS);
    if (fd_aht20 < 0) {
        perror("Error al abrir el bus I2C para AHT20");
        return -1;
    }

    if (aht20_init(fd_aht20) < 0) {
        return -1;
    }

    // Configuración para LM35
    int lm35_value;
    float lm35_volts, lm35_temperature;

    float humidity, temperature;

    while (1) {
        // Leer AHT20
        if (aht20_read(fd_aht20, &humidity, &temperature) == 0) {
            printf("AHT20 -> Temperatura: %.2f ºC, Humedad: %.2f %%\n", temperature, humidity);
        } else {
            printf("Error al leer datos del AHT20\n");
        }

        // Leer LM35
        if (spiadc_config_transfer(SINGLE_ENDED_CH0, &lm35_value) >= 0) {
            lm35_volts = 3.3 * lm35_value / 1023;
            lm35_temperature = lm35_volts * 1000 / 10;
            printf("LM35 -> Temperatura: %.2f ºC\n", lm35_temperature);
        } else {
            printf("Error al leer datos del LM35\n");
        }

        sleep(intervalo);
    }

    return 0;
    
	// Llamar a la función para enviar los datos
	//http(id_sensor, valor);
}

   

