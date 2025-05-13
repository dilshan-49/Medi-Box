#include <dashboardManager.h>

// Create WiFi and MQTT client instances
WiFiClient espClient;
PubSubClient mqttClient(espClient);

// Buffer for publishing messages
char mqttBuffer[100];
int LDR_sampletime = 5;
int LDR_updatetime = 120;
int LDR_samplecount = 0;




// MQTT callback function
void handleMQTTCallback(char* topic, byte* payload, unsigned int length) {
  // Create a null-terminated string from the payload
  char message[length + 1];
  memcpy(message, payload, length);
  message[length] = '\0';
  
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  Serial.println(message);

  
  // Process servo control commands
  if (strcmp(topic, TOPIC_UPDATE_TIME) == 0) {
    LDR_updatetime = atoi(message);
    LDR_updatetime = LDR_updatetime * 60;
    Serial.print("LDR update time set to: ");
    Serial.println(LDR_updatetime);
  }

  if (strcmp(topic, TOPIC_SAMPLE_TIME) == 0) {
    LDR_sampletime = atoi(message);
    LDR_sampletime = LDR_sampletime * 60;
    Serial.print("LDR sample time set to: ");
    Serial.println(LDR_sampletime);
  }

  if (strcmp(topic, TOPIC_SAMPLE_TIME) == 0) {
    LDR_samplecount = atoi(message);
    Serial.print("LDR sample count set to: ");
    Serial.println(LDR_samplecount);
  }



}

// Set up MQTT connection and settings
void setupMQTT() {
  // Configure MQTT server and port
  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  mqttClient.setCallback(handleMQTTCallback);
  
  // Set a larger buffer size for MQTT (optional)
  mqttClient.setBufferSize(512);
  
  // Try to connect to MQTT broker
  reconnectMQTT();
  
  // Publish initial status
  publishMediboxStatus("online");
  publishLightLevel(readLDR());
}

// Reconnect to MQTT broker if connection is lost
void reconnectMQTT() {
  // Loop until we're reconnected
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    
    // Attempt to connect with client ID and optional username/password
    if (mqttClient.connect(MQTT_CLIENT_ID, MQTT_USERNAME, MQTT_PASSWORD)) {
      Serial.println("connected");
      
      // Subscribe to command topic to receive commands from the dashboard
      mqttClient.subscribe(TOPIC_SERVO_CONTROL);
      
      // Subscribe to servo control topic
      mqttClient.subscribe(TOPIC_SERVO_CONTROL);
      
      // Notify that Medibox is online
      publishMediboxStatus("online");
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

// Publish temperature and humidity to dashboard
void publishTemperatureAndHumidity(float temperature, float humidity) {
  // Publish temperature
  snprintf(mqttBuffer, sizeof(mqttBuffer), "%.1f", temperature);
  mqttClient.publish(TOPIC_TEMPERATURE, mqttBuffer);
  
  // Publish humidity
  snprintf(mqttBuffer, sizeof(mqttBuffer), "%.1f", humidity);
  mqttClient.publish(TOPIC_HUMIDITY, mqttBuffer);
}


void publishTime(int hours, int minutes, int seconds) {
  snprintf(mqttBuffer, sizeof(mqttBuffer), "{\"hours\": %d, \"minutes\": %d, \"seconds\": %d}",
    hours, minutes, seconds);
    
  mqttClient.publish(TOPIC_TIME, mqttBuffer);
}

// Publish LDR light level to dashboard
void publishLightLevel(int lightValue) {
  snprintf(mqttBuffer, sizeof(mqttBuffer), "%d", lightValue);
  mqttClient.publish(TOPIC_LIGHT_LEVEL, mqttBuffer);
}

// Process commands received from dashboard
void processMQTTCommand(const char* command) {
  // Process different commands
  if (strcmp(command, "toggle_alarm1") == 0) {
    alarm_enabled[0] = !alarm_enabled[0];
    publishAlarmStatus(alarm_triggered[0], 1);
  } 
  else if (strcmp(command, "toggle_alarm2") == 0) {
    alarm_enabled[1] = !alarm_enabled[1];
    publishAlarmStatus(alarm_triggered[1], 2);
  }
  else if (strcmp(command, "auto_light_control_on") == 0) {
    controlBoxBasedOnLight(true);
  }
  else if (strcmp(command, "auto_light_control_off") == 0) {
    // Just disable auto control, don't change box position
    // The box will need manual control via MQTT or buttons
  }
  else if (strcmp(command, "request_status") == 0) {
    // Send all status information to dashboard
    TempAndHumidity data = dhtSensor.getTempAndHumidity();
    publishTemperatureAndHumidity(data.temperature, data.humidity);
    publishAlarmStatus(alarm_triggered[0], 1);
    publishAlarmStatus(alarm_triggered[1], 2);
    publishTime(hours, minutes, seconds);
    publishLightLevel(readLDR());
  }
  // Add more command handlers as needed
}

// Main loop function for MQTT - call this in your main loop
void loopMQTT() {
  // Check if connected to MQTT
  if (!mqttClient.connected()) {
    reconnectMQTT();
  }
  
  // Process MQTT messages
  mqttClient.loop();
  
  // Periodically publish data to dashboard
  static unsigned long lastTempUpdate = 0;
  static unsigned long lastLDRUpdate = 0;
  static unsigned long lastLDRSample = 0;
  static int lightLevel = 0;
  unsigned long currentMillis = millis();
  
  // Update temperature and time every 1 seconds
  if (currentMillis - lastTempUpdate >= 1000) {  // Update every 1 seconds
    lastTempUpdate = currentMillis;
    
    // Publish current temperature and humidity
    TempAndHumidity data = dhtSensor.getTempAndHumidity();
    publishTemperatureAndHumidity(data.temperature, data.humidity);
    
  }
  


  // Update LDR readings and box state every 2 minutes
  if (currentMillis - lastLDRUpdate >= LDR_updatetime*1000) {  // Update every 3 seconds
    lastLDRUpdate = currentMillis;
    lastLDRSample = currentMillis;
    // Publish light level
    lightLevel = lightLevel+readLDR();
    LDR_samplecount++;


    lightLevel = lightLevel / LDR_samplecount;
    publishLightLevel(lightLevel);


    lightLevel = 0;  // Reset light level for next sample
    LDR_samplecount = 0;  // Reset sample count for next sample
    // Publish box state (in case it was changed by other means)
  }
  else if (currentMillis - lastLDRSample >= LDR_sampletime*1000) {  // Update every 3 seconds
    lastLDRSample = currentMillis;
    
    // Sample LDR value
    lightLevel = lightLevel+readLDR();
    LDR_samplecount++;
    
    // Publish box state (in case it was changed by other means)
  }

}