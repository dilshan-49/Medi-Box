#include <dashboardManager.h>

// Create WiFi and MQTT client instances
WiFiClientSecure espClient;
PubSubClient mqttClient(espClient);

// Buffer for publishing messages
char mqttBuffer[100];
int LDR_sampletime = 5;
int LDR_updatetime = 120;
int LDR_samplecount = 0;

int theta = 30;
float gamma_factor = 0.75;
float med_temp = 30.0;




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
    float _ = atof(message);
    LDR_updatetime = int(_ * 60);
    Serial.print("LDR update time set to: ");
    Serial.println(LDR_updatetime);
  }

  if (strcmp(topic, TOPIC_SAMPLE_TIME) == 0) {
    LDR_sampletime = atoi(message);
    Serial.print("LDR sample time set to: ");
    Serial.println(LDR_sampletime);
  }

  if (strcmp(topic, TOPIC_THETA) == 0) {
    theta = atoi(message);
    Serial.print("Theta set to: ");
    Serial.println(theta);
    
  }

  if (strcmp(topic, TOPIC_GAMMA) == 0) {
    gamma_factor = atof(message);
    Serial.print("Gamma set to: ");
    Serial.println(gamma_factor);
    
  }

  if (strcmp(topic, TOPIC_MED_TEMP) == 0) {
    med_temp = atof(message);
    Serial.print("Medibox temperature set to: ");
    Serial.println(med_temp);
    
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
  
  publishLightLevel(readLDR());
}

// Reconnect to MQTT broker if connection is lost
void reconnectMQTT() {
  espClient.setInsecure();
  // Loop until we're reconnected
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    
    // Attempt to connect with client ID and optional username/password
    if (mqttClient.connect(MQTT_CLIENT_ID, MQTT_USERNAME, MQTT_PASSWORD)) {
      Serial.println("connected");
      
      // Subscribe to Servo control topics
      mqttClient.subscribe(TOPIC_GAMMA);
      mqttClient.subscribe(TOPIC_MED_TEMP);
      mqttClient.subscribe(TOPIC_THETA);
      
      // Subscribe to LDR topics
      mqttClient.subscribe(TOPIC_SAMPLE_TIME);
      mqttClient.subscribe(TOPIC_UPDATE_TIME);
      
    } 
    
    else {
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

// Publish LDR light level to dashboard
void publishLightLevel(float lightValue) {
  snprintf(mqttBuffer, sizeof(mqttBuffer), "%.2f", lightValue);
  mqttClient.publish(TOPIC_LIGHT_LEVEL, mqttBuffer);
}

// Publish servo angle to dashboard
void publishServoAngle(int angle) {
  snprintf(mqttBuffer, sizeof(mqttBuffer), "%d", angle);
  mqttClient.publish(TOPIC_SERVO_CONTROL, mqttBuffer);
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
  static float lightLevel = 0;
  unsigned long currentMillis = millis();
  
  // Update temperature and time every 1 seconds
  if (currentMillis - lastTempUpdate >= 1000) {  // Update every 1 seconds
    lastTempUpdate = currentMillis;
    
    // Publish current temperature and humidity
    TempAndHumidity data = dhtSensor.getTempAndHumidity();
    publishTemperatureAndHumidity(data.temperature, data.humidity);
    
    int angle = mediBoxServo.read();
    publishServoAngle(angle);
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