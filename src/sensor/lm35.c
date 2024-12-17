#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <string.h>

#define SINGLE_ENDED_CH0 0 // Canales configurados como entrada única

int verbose = 1;

static char *cntdevice = "/dev/spidev0.0";

// -----------------------------------------------------------------------------------------------

static void pabort(const char *s)
{
	perror(s);
	abort();
}

// -----------------------------------------------------------------------------------------------

static void spiadc_config_tx(int conf, uint8_t tx[2])
{
	tx[0] = 0x01; // Configuración del canal
	tx[1] = (conf & 0x07); // Espacio para los datos del ADC
}

// -----------------------------------------------------------------------------------------------

static int spiadc_transfer(int fd, uint8_t bits, uint32_t speed, uint16_t delay, uint8_t tx[2], uint8_t *rx, int len)
{
    int ret, value;
    struct spi_ioc_transfer tr = {
        .tx_buf = (unsigned long)tx,
        .rx_buf = (unsigned long)rx,
        .len = len * sizeof(uint8_t),  // En este caso, len = 2
        .delay_usecs = delay,
        .speed_hz = speed,
        .bits_per_word = bits,
    };

    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);

    if (ret < 1)
        pabort("can't send spi message");

    // Extraer los bits significativos del ADC (10 bits)
    value = ((rx[0] & 0x1F) << 8) | rx[1]; // Extrae los 10 bits significativos

    if (verbose) {
        printf("SPI RX: ");
        for (int i = 0; i < len; i++) {
            printf("0x%02x ", rx[i]);
        }
        printf("--> Valor decodificado: %d\n", value);
    }

    return value; // Devuelve el valor decodificado
}


// -----------------------------------------------------------------------------------------------

static int spiadc_config_transfer(int conf, int *value)
{
	int ret = 0;
	int fd;
	uint8_t rx[2];
	uint8_t tx[2];
	char buffer[255];

	/* SPI parameters */
	char *device = cntdevice;
	uint8_t mode = SPI_MODE_0;
	uint8_t bits = 8;
	uint32_t speed = 500000;
	uint16_t delay = 0;

	/* Open device */
	fd = open(device, O_RDWR);
	if (fd < 0) {
		sprintf(buffer, "No se puede abrir el dispositivo (%s)", device);
		pabort(buffer);
	}

	/* SPI mode */
	ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
	if (ret == -1)
		pabort("No se puede establecer el modo SPI");

	ret = ioctl(fd, SPI_IOC_RD_MODE, &mode);
	if (ret == -1)
		pabort("No se puede leer el modo SPI");

	/* Bits per word */
	ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
	if (ret == -1)
		pabort("No se puede establecer los bits por palabra");

	ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
	if (ret == -1)
		pabort("No se puede leer los bits por palabra");

	/* Max speed HZ */
	ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		pabort("No se puede establecer la velocidad máxima");

	ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		pabort("No se puede leer la velocidad máxima");

	/* Build data to transfer */
	spiadc_config_tx(conf, tx);

	/* SPI adc transfer */
	ret = spiadc_transfer(fd, bits, speed, delay, tx, rx, 3);
	if (ret < 1)
		pabort("Error al enviar el mensaje SPI");

	close(fd);

	/* Interpretación del resultado */
	*value = (rx[0] << 6) | rx[1] >> 2; // 10 bits útiles del ADC

	return ret;
}

// -----------------------------------------------------------------------------------------------

int main(int argc, char *argv[])
{
	int ret = 0, value_int;
	float value_volts;
	float temperatura;

	ret = spiadc_config_transfer(SINGLE_ENDED_CH0, &value_int);

	printf("Valor leído (0-1023): %d\n", value_int);
	value_volts = 3.3 * value_int / 1023;
	temperatura = value_volts*1000/10;
	printf("Voltaje: %.3f V\n", value_volts);
	printf("temperatura %.3f ºC\n", temperatura);

	return ret;
}
