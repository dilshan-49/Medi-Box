#ifndef DASHBOARDMANAGER_H
#define DASHBOARDMANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <config.h>

// MQTT Settings
#define MQTT_SERVER "test.mosquitto.org"  // Public MQTT broker (replace with your broker)
#define MQTT_PORT 1883
#define MQTT_CLIENT_ID "MediBoxClient"
#define MQTT_USERNAME ""  // If MQTT authentication is required
#define MQTT_PASSWORD ""  // If MQTT authentication is required

// MQTT Topics
#define TOPIC_TEMPERATURE "medibox/temp"
#define TOPIC_HUMIDITY "medibox/humidity"
#define TOPIC_THETA "medibox/theta"
#define TOPIC_GAMMA "medibox/gamma"  // Topic for Medibox status
#define TOPIC_MED_TEMP "medibox/med_temp"  // Topic for Medibox temperature
#define TOPIC_TIME "medibox/time"
#define TOPIC_LIGHT_LEVEL "medibox/light"  // Topic for LDR readings
#define TOPIC_SERVO_CONTROL "medibox/servo"      // Topic for servo control commands
#define TOPIC_UPDATE_TIME "medibox/update_time"  // Topic for time update commands
#define TOPIC_SAMPLE_TIME "medibox/sample_time"  // Topic for time sampling commands




// Function declarations
void setupMQTT();
void reconnectMQTT();
void handleMQTTCallback(char* topic, byte* payload, unsigned int length);
void publishTemperatureAndHumidity(float temperature, float humidity);
void publishTime(int hours, int minutes, int seconds);
void publishLightLevel(int lightValue);
void processMQTTCommand(const char* command);
void loopMQTT();

#endif 