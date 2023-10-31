#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <LiquidCrystal.h>
LiquidCrystal lcd(16, 5, 4, 0, 2, 14);
// (lcd 1,5,16 - GND ESP8266) , (2,15 - 5V), (4-D0),(6-D1),(11-D2),(12-D3),(13-D4),(14-D5)

#define ssid "SkyNet"
#define password "12345678123"
#define mqtt_server "broker.mqttdashboard.com"
const uint16_t mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

void setup() 
{
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port); 
  client.setCallback(callback);
  
  lcd.begin(16, 2);
delay(1000);
}
void setup_wifi() 
{
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) 
{
  Serial.print("Co tin nhan moi tu topic:");
  Serial.print(topic);
  Serial.println();
  lcd.clear();
  {
  for(int i=0;i<length;i++)
  {
    Serial.print((char)payload[i]);  
    lcd.setCursor(0, 0);
    lcd.print("Temp/Humi"); // Start Print text to Line 1
    lcd.setCursor(i, 1);
    lcd.write(length);
    delay(500);
    }
  }
  Serial.println();
}
void reconnect() 
{
  while (!client.connected())
  {
    if (client.connect("TamDaoMinh","31072001","clientId-9ZbLAmgqom"))
    {
      Serial.println("Đã kết nối:");
      client.subscribe("nodeWiFi32/dht11/temperature");
      client.subscribe("nodeWiFi32/dht11/humidity");

    }
    else 
    {
      Serial.print("Lỗi:, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}
void loop() 
{
  if (!client.connected())
    reconnect();
  client.loop();
  if(Serial.available() > 0)
  {
  }
}
