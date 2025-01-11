#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>

#define I2C_ADDRESS 0x70 // Dirección I2C del sensor
#define BUFFER_SIZE 7    // Tamaño de datos recibidos del sensor

uint8_t calculate_crc(uint8_t *data, int length);

void VZ89TEReadData(int file, uint8_t request, uint8_t *data)
{
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

uint8_t calculate_crc(uint8_t *data, int length)
{
    uint8_t crc = 0;
    for (int i = 0; i < length; i++) {
		crc += data[i];
    }
    crc = (crc + (crc / 0x100)) & 0xFF; // Reducir a 8 bits
    return 0xFF - crc;
}

int main(void)
{
    int file;
    char *i2c_device = "/dev/i2c-1";
    uint8_t data[BUFFER_SIZE];
    uint16_t VOCvalue, CO2value, R0Value;
    uint32_t ResistorValue;

    // Abrir el bus I2C
    file = open(i2c_device, O_RDWR);
    if (file < 0) {
		perror("Error al abrir el bus I2C");
		return 1;
    }

    // Configurar la dirección del sensor
    if (ioctl(file, I2C_SLAVE, I2C_ADDRESS) < 0) {
		perror("No se pudo conectar con el dispositivo");
		return 1;
    }

    // Leer fecha y revisión
    VZ89TEReadData(file, 0x0D, data);
    printf("Fecha: %02d-%02d-%02d, Revisión: %d\n", data[2], data[1], data[0], data[3]);

    // Leer valor de calibración R0
    VZ89TEReadData(file, 0x10, data);
    R0Value = ((data[1] & 0x3F) << 8) | data[0];
    printf("Valor de calibración R0: %d kohm\n", R0Value);

    // Leer estado y valores de VOC y CO2
    VZ89TEReadData(file, 0x0C, data);
    VOCvalue = (data[0] - 13) * (1000 / 229);
    CO2value = (data[1] - 13) * (1600 / 229) + 400;
    ResistorValue = 10 * (data[4] + (256 * data[3]) + (65536 * data[2]));
    printf("VOC: %d ppb\n", VOCvalue);
    printf("CO2: %d ppm\n", CO2value);
    printf("Resistor: %d ohm\n", ResistorValue);

    // Cerrar el archivo I2C
    close(file);
    return 0;
}
