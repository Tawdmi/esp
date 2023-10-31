#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

const char* ssid = "Truong TH Van Phuc";
const char* password = "kinhchaoquykhach321";

#define MQTT_SERVER "broker.mqttdashboard.com"
#define MQTT_PORT 1883
#define MQTT_USER "TamDaoMinh"
#define MQTT_PASSWORD "31072001"
#define MQTT_TEMPERATURE_TOPIC "nodeWiFi32/dht11/temperature"
#define MQTT_HUMIDITY_TOPIC "nodeWiFi32/dht11/humidity"
#define MQTT_LED_TOPIC "nodeWiFi32/led"

#define LEDPIN 12
#define DHTPIN 14
#define DHTTYPE DHT11

unsigned long previousMillis = 0; 
const long interval = 5000;
int current_ledState = LOW;
int last_ledState = LOW;

WiFiClient wifiClient;
PubSubClient client(wifiClient);
DHT dht(DHTPIN, DHTTYPE);

void setup_wifi() {
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  randomSeed(micros());
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void connect_to_broker() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "nodeWiFi32";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str(), MQTT_USER, MQTT_PASSWORD)) {
      Serial.println("connected");
      client.subscribe(MQTT_LED_TOPIC);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 2 seconds");
      delay(2000);
    }
  }
}

void callback(char* topic, byte *payload, unsigned int length) {
  Serial.println("-------new message from broker-----");
  Serial.print("topic: ");
  Serial.println(topic);
  Serial.print("message: ");
  Serial.write(payload, length);
  Serial.println();
  if (*payload == '1') current_ledState = HIGH;
  if (*payload == '0') current_ledState = LOW;
}

void setup() {

  pinMode(12, OUTPUT);
  Serial.begin(115200);
  Serial.setTimeout(500);
  setup_wifi();
  client.setServer(MQTT_SERVER, MQTT_PORT );
  client.setCallback(callback);
  connect_to_broker();
  dht.begin();
  digitalWrite(LEDPIN, current_ledState);
 
}

void loop() {
  client.loop();
  if (!client.connected()) {
    connect_to_broker();
  }
  if (last_ledState != current_ledState) {
    last_ledState = current_ledState;
    digitalWrite(LEDPIN, current_ledState);
    Serial.print("LED state is ");
    Serial.println(current_ledState);
  }
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    
    if (isnan(h) || isnan(t)) {
      Serial.println(F("Failed to read from DHT sensor!"));
      return;
    } else {
      client.publish(MQTT_TEMPERATURE_TOPIC, String(t).c_str());
      client.publish(MQTT_HUMIDITY_TOPIC, String(h).c_str());
      previousMillis = currentMillis;
        delay(500);
    }
  }
}
