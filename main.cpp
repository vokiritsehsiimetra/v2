#include <iostream>
#include "bmp280.h"
#include <mqtt/async_client.h>  // Paho MQTT header

#define ADDRESS     "tcp://broker_address:1883"
#define CLIENTID    "RaspberryPiZero"
#define TOPIC       "sensor/bmp280"
#define QOS         1
#define TIMEOUT     10000L

bmp280_t bmp280;  // Assuming this is your sensor object

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
    // Initialize BMP280 sensor (assuming this is already implemented in bmp280.cpp)
    if (!bmp280_init(&bmp280)) {
        std::cerr << "Failed to initialize BMP280!" << std::endl;
        return -1;
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
        float temperature, pressure;
        if (bmp280_read(&bmp280, &temperature, &pressure)) {
            std::cout << "Temperature: " << temperature << " C, Pressure: " << pressure << " hPa" << std::endl;
            publish_data(temperature, pressure);
        } else {
            std::cerr << "Failed to read sensor data!" << std::endl;
        }

        // Wait for some time before next read
        sleep(5);
    }

    // Disconnect from MQTT broker
    client.disconnect()->wait();
    return 0;
}
