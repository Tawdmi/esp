#include <WiFi.h>
#include <HTTPClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>
// Data wire is connected to GPIO15
#define ONE_WIRE_BUS 4
// Setup a oneWire instance to communicate with a OneWire device
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature sensor
DallasTemperature sensors(&oneWire);

DeviceAddress sensor1 = { 0x28, 0xFF, 0x64, 0x1E, 0x84, 0x25, 0xED, 0xBD };
//DeviceAddress sensor2 = { 0x28, 0xFF, 0xB4, 0x6, 0x33, 0x17, 0x3, 0x4B };
//DeviceAddress sensor3= { 0x28, 0xFF, 0xA0, 0x11, 0x33, 0x17, 0x3, 0x96 };



const char * ssid = "Xuan Ba";
const char * password = "vansuthanhcong";


String GOOGLE_SCRIPT_ID = "AKfycbwblKB6_e3JUdNmTpQMwatI7cyDPNjahQO_zkoOdNNSXc4Ah-9O833GMmFTl43cA8GK";


const int sendInterval = 50;

WiFiClientSecure client;


void setup() {
  sensors.begin();
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
  }
  Serial.println("OK");
}
void loop() {
  Serial.print("Requesting temperatures...");
  sensors.requestTemperatures(); // Send the command to get temperatures
  Serial.println("DONE");

  Serial.print("Sensor 1(*C): ");
  Serial.print(sensors.getTempC(sensor1));
  Serial.print(" Sensor 1(*F): ");
  Serial.println(sensors.getTempF(sensor1));
  
  write_google_sheet( "value1=" + String(sensors.getTempC(sensor1)) + "&value2=" + String(sensors.getTempF(sensor1)));
  delay(1000);

}


void write_google_sheet(String params) {
  HTTPClient http;
  String url = "https://script.google.com/macros/s/" + GOOGLE_SCRIPT_ID + "/exec?" + params;
  Serial.println(url);
  Serial.println("Updating Temperature & Humidity Status");
  http.begin(url.c_str());
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
  int httpCode = http.GET();
  Serial.print("HTTP Status Code: ");
  Serial.println(httpCode);

  String payload;
  if (httpCode > 0) {
    payload = http.getString();
    Serial.println("Payload: " + payload);
  }
  http.end();
}
