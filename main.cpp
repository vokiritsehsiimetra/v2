#include <iostream>
#include "bmp280.h"
#include <mqtt/async_client.h>  // Paho MQTT header

#define ADDRESS     "tcp://broker_address:1883"
#define CLIENTID    "RaspberryPiZero"
#define TOPIC       "sensor/bmp280"
#define QOS         1
#define TIMEOUT     10000L

// Initialize BMP280 object with I2C bus 1 and I2C address 0x76
BMP280 bmp280(1, 0x76);

// MQTT configuration
mqtt::async_client client(ADDRESS, CLIENTID);
mqtt::connect_options connOpts;

void publish_data(float temperature, float pressure) {
    std::string payload = "{\"temperature\": " + std::to_string(temperature) + ", \"pressure\": " + std::to_string(pressure) + "}";
    mqtt::message_ptr pubmsg = mqtt::make_message(TOPIC, payload);
    pubmsg->set_qos(QOS);
    client.publish(pubmsg);
}

int main() {
    // Initialize BMP280 sensor
    if (!bmp280.begin()) {
        std::cerr << "Failed to initialize BMP280" << std::endl;
        return 1;
    }

    // Connect to MQTT broker
    try {
        client.connect(connOpts)->wait();
        std::cout << "Connected to MQTT broker" << std::endl;
    } catch (const mqtt::exception& exc) {
        std::cerr << "MQTT connection failed: " << exc.what() << std::endl;
        return 1;
    }

    // Main loop to read sensor data and publish via MQTT
    while (true) {
        float temperature = bmp280.readTemperature();
        float pressure = bmp280.readPressure();

        std::cout << "Temperature: " << temperature << " °C, Pressure: " << pressure << " hPa" << std::endl;

        // Publish data to MQTT
        publish_data(temperature, pressure);

        // Sleep for 5 seconds before reading again
        sleep(5);
    }

    // Disconnect from MQTT broker
    client.disconnect()->wait();
    return 0;
}
