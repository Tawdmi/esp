#define BLYNK_PRINT Serial

/* Fill in information from Blynk Device Info here */
#define BLYNK_TEMPLATE_ID "TMPL6tDgvAOnm"
#define BLYNK_TEMPLATE_NAME "ESP32 3Relay 1Pzem004t"
#define BLYNK_AUTH_TOKEN "5_zb968WA9nIFxh9ZQMUGkMsVPoh6Yo2"


#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

#include <PZEM004Tv30.h>

#if !defined(PZEM_RX_PIN) && !defined(PZEM_TX_PIN)
#define PZEM_RX_PIN 16
#define PZEM_TX_PIN 17
#endif

#if !defined(PZEM_SERIAL)
#define PZEM_SERIAL Serial2
#endif

#if defined(ESP32)
PZEM004Tv30 pzem(PZEM_SERIAL, PZEM_RX_PIN, PZEM_TX_PIN);
#elif defined(ESP8266)

//PZEM004Tv30 pzem(Serial1);
#else

PZEM004Tv30 pzem(PZEM_SERIAL);
#endif

#define RELAY1 05
#define RELAY2 18
#define RELAY3 19
#define BUZZER 21

const int resetButtonPin = 23; // Reset toàn bộ hệ thống

char ssid[] = "Tang 2";
char pass[] = "Minhhai05";

BLYNK_WRITE(V1) {
  int pinValue = param.asInt();
  if (pinValue == 1) {
    if (digitalRead(RELAY2) == HIGH || digitalRead(RELAY3) == HIGH) {
      Blynk.virtualWrite(V1, 0);
      digitalWrite(BUZZER, HIGH);
      delay(1000);
      digitalWrite(BUZZER, LOW);
    } else {
      digitalWrite(RELAY1, HIGH);
    }
  } else {
    digitalWrite(RELAY1, LOW);
  }
}

BLYNK_WRITE(V2) {
  int pinValue = param.asInt();
  if (pinValue == 1) {
    if (digitalRead(RELAY1) == HIGH || digitalRead(RELAY3) == HIGH) {
      Blynk.virtualWrite(V2, 0);
      digitalWrite(BUZZER, HIGH);
      delay(1000);
      digitalWrite(BUZZER, LOW);
    } else {
      digitalWrite(RELAY2, HIGH);
    }
  } else {
    digitalWrite(RELAY2, LOW);
  }
}

BLYNK_WRITE(V3) {
  int pinValue = param.asInt();
  if (pinValue == 1) {
    if (digitalRead(RELAY1) == HIGH || digitalRead(RELAY2) == HIGH) {
      Blynk.virtualWrite(V3, 0);
      digitalWrite(BUZZER, HIGH);
      delay(1000);
      digitalWrite(BUZZER, LOW);
    } else {
      digitalWrite(RELAY3, HIGH);
    }
  } else {
    digitalWrite(RELAY3, LOW);
  }
}

BLYNK_WRITE(V10) {  // Button Widget chọn V0
  if (param.asInt() == 1) {  //nếu nhấn nút reset thì gọi hàm
    pzem.resetEnergy(); // reset PZEM
  } else {
  }
}

void setup()
{
  // Debug console
  Serial.begin(115200);

  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);
  pinMode(RELAY3, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(resetButtonPin, INPUT_PULLUP);

  digitalWrite(RELAY1, LOW);
  digitalWrite(RELAY2, LOW);
  digitalWrite(RELAY3, LOW);
  digitalWrite(BUZZER, LOW);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
}

void loop()
{
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Disconnected from WiFi. Attempting reconnection...");
    Blynk.disconnect();
    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("Reconnected to WiFi");
    Blynk.connect();
  }
  if (digitalRead(resetButtonPin) == LOW) {
    ESP.restart();        //ESP reset
  }

  Serial.print("Custom Address:");
  Serial.println(pzem.readAddress(), HEX);

  // Read the data from the sensor
  float voltage = pzem.voltage();
  if (!isnan(voltage)) {
    Blynk.virtualWrite(V4, voltage);
    Serial.print("Voltage: ");      Serial.print(voltage);      Serial.println("V");
  }

  float current = pzem.current();
  if (!isnan(current)) {
    Blynk.virtualWrite(V5, current);
    Serial.print("Current: ");      Serial.print(current);      Serial.println("A");
  }

  float power = pzem.power();
  if (!isnan(power)) {
    Blynk.virtualWrite(V6, power);
    Serial.print("Power: ");        Serial.print(power);        Serial.println("W");
  }

  float energy = pzem.energy();
  if (!isnan(energy)) {
    Blynk.virtualWrite(V7, energy);
    Serial.print("Energy: ");       Serial.print(energy, 3);     Serial.println("kWh");

  }

  float frequency = pzem.frequency();
  if (!isnan(frequency)) {
    Blynk.virtualWrite(V8, frequency);
    Serial.print("Frequency: ");    Serial.print(frequency, 1); Serial.println("Hz");
  }

  float pf = pzem.pf();
  if (!isnan(pf)) {
    Blynk.virtualWrite(V9, pf);
    Serial.print("PF: ");           Serial.println(pf);
  }
  Serial.println();
  Blynk.run();
}
