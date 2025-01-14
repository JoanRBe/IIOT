#include <iostream>
#include <string>
#include <cstdlib>
#include <unistd.h>
#include <mqtt/callback.h>
#include "mqtt.h"
#include <mqtt/async_client.h>  // Ruta estándar de inclusión de la librería MQTT

extern float lm35_temperature;
extern float humidity;
extern char VOC[10];
extern char CO2[10];

// Configuración del cliente MQTT
const std::string SERVER_ADDRESS("192.168.11.110:1883"); //"tcp://192.168.11.110:8123" Dirección IP de tu Home Assistant MQTT
const std::string CLIENT_ID("RaspberryPi25_SensorClient");  //Identificador del cliente MQTT
const std::string TOPICTMP("sensor/lm35/temperature");  //Tópico donde se enviarán los datos
const std::string TOPICHDT("sensor/aht20/humidity");
const std::string TOPICVOC("sensor/vz89te/voc");
const std::string TOPICCO2("sensor/vz89te/co2");

// Callback para manejar eventos del cliente MQTT
class callback : public virtual mqtt::callback
{
public:
    // Conexión exitosa
    void connected(const std::string& cause) override {
        std::cout << "Conectado al broker MQTT: " << cause << std::endl;
    }

    // Mensaje recibido
    void message_arrived(mqtt::const_message_ptr msg) override {
        // Imprimir el mensaje recibido y el tópico usando el operador <<
        std::cout << "Mensaje recibido: " << msg->to_string() << " en el tópico " << msg->get_topic() << std::endl;
    }

    // Entrega completada
    void delivery_complete(mqtt::delivery_token_ptr token) override {
        std::cout << "Entrega del mensaje completada." << std::endl;
    }
};

void homeassistant(float lm35_temperature, float humidity, const std::string& VOC, const std::string& CO2){
    // Crear un cliente MQTT
    mqtt::async_client client(SERVER_ADDRESS, CLIENT_ID);
    
    // Configurar el callback
    callback cb;
    client.set_callback(cb);

    // Configuración de las opciones de conexión (usuario y contraseña)
    mqtt::connect_options connOpts;
    connOpts.set_user_name("iiot_grup4");  // Sustituye por el usuario correcto
    connOpts.set_password("raspberryEUSS");  // Sustituye por la contraseña correcta

    // Intentar conectar al broker con autenticación
    try {
        std::cout << "Intentando conectar..." << std::endl;
        client.connect(connOpts)->wait();  // Conectar al broker con las opciones
        std::cout << "Conectado exitosamente!" << std::endl;

        // Publicar los datos del sensor cada 10 segundos
        while (true) {
            // Simula la lectura de un sensor (por ejemplo, temperatura aleatoria)
            std::string payload1 = std::to_string(lm35_temperature); // Convertir el valor a string
            std::string payload2 = std::to_string(humidity);
            std::string payload3 = VOC;  // VOC es un char[] convertido a string
            std::string payload4 = CO2; // CO2 es un char[] convertido a string

            // Publicar en el tópico MQTT
            std::cout << "Publicando " << payload1 << " en el topico " << TOPICTMP << std::endl;
            client.publish(TOPICTMP, payload1.c_str(), payload1.length(), 0, false);  // QoS=0, no-retained

            // Esperar 10 segundos antes de la siguiente publicación
            sleep(10);

            // Publicar en el tópico MQTT
            std::cout << "Publicando " << payload2 << " en el topico " << TOPICHDT << std::endl;
            client.publish(TOPICHDT, payload2.c_str(), payload2.length(), 0, false);  // QoS=0, no-retained

            // Esperar 10 segundos antes de la siguiente publicación
            sleep(10);
            
            // Publicar en el tópico MQTT
            std::cout << "Publicando " << payload3 << " en el topico " << TOPICVOC << std::endl;
            client.publish(TOPICVOC, payload3.c_str(), payload3.length(), 0, false);  // QoS=0, no-retained

            // Esperar 10 segundos antes de la siguiente publicación
            sleep(10);
            
            // Publicar en el tópico MQTT
            std::cout << "Publicando " << payload4 << " en el topico " << TOPICCO2 << std::endl;
            client.publish(TOPICCO2, payload4.c_str(), payload4.length(), 0, false);  // QoS=0, no-retained

            // Esperar 10 segundos antes de la siguiente publicación
            sleep(10);
        }

        // Desconectar (aunque este código nunca se ejecutará por la estructura de la conexión)
        client.disconnect()->wait();
    } catch (const mqtt::exception& exc) {
        std::cout << "Error: " << exc.what() << std::endl;
        return;
    }

    return;
}
