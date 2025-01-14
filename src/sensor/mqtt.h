#ifndef MQTT_H
#define MQTT_H

#include <string>

void homeassistant(float lm35_temperature, float humidity, const std::string& VOC, const std::string& CO2);

#endif
