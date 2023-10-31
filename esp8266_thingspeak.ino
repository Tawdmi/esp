#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#

const char *SSID = "Truong TH Van Phuc";
const char *PASS = "kinhchaoquykhach";

PubSubClient mqttClient(wifiClient); 
char *mqttServer = "broker.hivemq.com";
int mqttPort = 1883;
void setupMQTT() {
  mqttClient.setServer(mqttServer, mqttPort);
  // set the callback function
  mqttClient.setCallback(callback);
}

void connectToWiFi() {
  Serial.print("Connectiog to ");
 
  WiFi.begin(SSID, PWD);
  Serial.println(SSID);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.print("Connected.");
  
}
