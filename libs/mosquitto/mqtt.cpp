#include <iostream>
#include <string>
#include <cstdlib>
#include <unistd.h>
#include <mqtt/callback.h>
#include <mqtt/async_client.h>  // Ruta estándar de inclusión de la librería MQTT

// Configuración del cliente MQTT
const std::string SERVER_ADDRESS("192.168.11.110:1883"); //"tcp://192.168.11.110:8123" Dirección IP de tu Home Assistant MQTT
const std::string CLIENT_ID("RaspberryPi25_SensorClient");  // Identificador del cliente MQTT
const std::string TOPIC("sensor/temperatura");  // Tópico donde se enviarán los datos

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

int main() {
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
            float temperatura = 25.0f + (rand() % 100) / 10.0f;  // Temperatura entre 25.0 y 34.9 grados
            std::string payload = std::to_string(temperatura);  // Convertir el valor a string

            // Publicar en el tópico MQTT
            std::cout << "Publicando " << payload << " en el topico " << TOPIC << std::endl;
            client.publish(TOPIC, payload.c_str(), payload.length(), 0, false);  // QoS=0, no-retained

            // Esperar 10 segundos antes de la siguiente publicación
            sleep(10);
        }

        // Desconectar (aunque este código nunca se ejecutará por la estructura de la conexión)
        client.disconnect()->wait();
    } catch (const mqtt::exception& exc) {
        std::cout << "Error: " << exc.what() << std::endl;
        return 1;
    }

    return 0;
}
