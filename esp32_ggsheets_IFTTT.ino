#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>

#define DHTPIN    14
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

const char * ssid = "Truong TH Van Phuc";
const char * password = "kinhchaoquykhach321";

String server = "http://maker.ifttt.com";
String eventName = "dht11_readings";
String IFTTT_Key = "gy0bRIuA1GPf6QE3aoh9H0_a47-4xaSQazlLswpHLPX";
String IFTTTUrl="https://maker.ifttt.com/trigger/dht11_readings/with/key/gy0bRIuA1GPf6QE3aoh9H0_a47-4xaSQazlLswpHLPX";

int value1;
int value2;
int value3;

void setup() {

  Serial.begin(115200);
  dht.begin();
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected !!!");

}
void sendDataToSheet(void)
{
  String url = server + "/trigger/" + eventName + "/with/key/" + IFTTT_Key + "?value1=" + String((int)value1) + "&value2="+String((int)value2) +"&value3=" + String((int)value3);  

  Serial.println(url);
  HTTPClient http;
  Serial.print("[HTTP] begin...\n");
  http.begin(url);
  Serial.print("[HTTP] GET...\n");
  int httpCode = http.GET();
  if(httpCode > 0) {
    Serial.printf("[HTTP] GET... code: %d\n", httpCode);
    if(httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println(payload);

    }
  } else {
    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }
  http.end();

}
void loop() {
  value1 = dht.readTemperature();
  value2 = dht.readHumidity();
  value3 = dht.readTemperature(true);

  Serial.print("Values are ");
  Serial.print(value1);
  Serial.print(' ');
  Serial.print(value2);
  Serial.print(' ');
  Serial.print(value3);
  Serial.print(' ');

  sendDataToSheet();
  delay(10000);


}
