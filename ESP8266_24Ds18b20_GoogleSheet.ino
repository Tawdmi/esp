#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <OneWire.h>
#include <DallasTemperature.h>

const char* ssid = "    "; //Ten WIFI
const char* password = "    "; //Mat khau WIFI

#define ONE_WIRE_BUS 4 //GPIO D2 ESP

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

DeviceAddress sensor1 = { 0x28, 0xFF, 0x64, 0x1E, 0x84, 0x25, 0xED, 0xBD };
DeviceAddress sensor2  = {       }; //Điền lần lượt các địa chỉ vừa quét được vào các sensor tương ứng
DeviceAddress sensor3  = {       };
DeviceAddress sensor4  = {       };
DeviceAddress sensor5  = {       };
DeviceAddress sensor6  = {       };
DeviceAddress sensor7  = {       };
DeviceAddress sensor8  = {       };
DeviceAddress sensor9  = {       };
DeviceAddress sensor10 = {       };
DeviceAddress sensor11 = {       };
DeviceAddress sensor12 = {       };
DeviceAddress sensor13 = {       };
DeviceAddress sensor14 = {       };
DeviceAddress sensor15 = {       };
DeviceAddress sensor16 = {       };
DeviceAddress sensor17 = {       };
DeviceAddress sensor18 = {       };
DeviceAddress sensor19 = {       };
DeviceAddress sensor20 = {       };
DeviceAddress sensor21 = {       };
DeviceAddress sensor22 = {       };
DeviceAddress sensor23 = {       };
DeviceAddress sensor24 = {       };

const char* host = "script.google.com";
const int httpsPort = 443;
//----------------------------------------

WiFiClientSecure client;

String GAS_ID = "AKfycbzaV4HQCrCfIU2Iy6p3HYMN_4DsGN2mKagGqmWC193S21vbpZLtEjW0sJAEmVYkKdpfqg"; //--> Spreadsheet script ID

void setup() {
  Serial.begin(115200);
  sensors.begin();

  WiFi.begin(ssid, password);
  Serial.println("");

  //----------------------------------------Wait for connection
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Successfully connected to : ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  //----------------------------------------

  client.setInsecure();
}

void loop() {

  Serial.print("Requesting temperatures...");
  sensors.requestTemperatures();
  Serial.println("DONE");

  Serial.print("Sensor 1(*C): ");
  Serial.println(sensors.getTempC(sensor1));

  Serial.print("Sensor 2(*C): ");
  Serial.println(sensors.getTempC(sensor2));

  Serial.print("Sensor 3(*C): ");
  Serial.println(sensors.getTempC(sensor3));

  Serial.print("Sensor 4(*C): ");
  Serial.println(sensors.getTempC(sensor4));

  Serial.print("Sensor 5(*C): ");
  Serial.println(sensors.getTempC(sensor5));

  Serial.print("Sensor 6(*C): ");
  Serial.println(sensors.getTempC(sensor6));

  Serial.print("Sensor 7(*C): ");
  Serial.println(sensors.getTempC(sensor7));

  Serial.print("Sensor 8(*C): ");
  Serial.println(sensors.getTempC(sensor8));

  Serial.print("Sensor 9(*C): ");
  Serial.println(sensors.getTempC(sensor9));

  Serial.print("Sensor 10(*C): ");
  Serial.println(sensors.getTempC(sensor10));

  Serial.print("Sensor 11(*C): ");
  Serial.println(sensors.getTempC(sensor11));

  Serial.print("Sensor 12(*C): ");
  Serial.println(sensors.getTempC(sensor12));

  Serial.print("Sensor 13(*C): ");
  Serial.println(sensors.getTempC(sensor13));

  Serial.print("Sensor 14(*C): ");
  Serial.println(sensors.getTempC(sensor14));

  Serial.print("Sensor 15(*C): ");
  Serial.println(sensors.getTempC(sensor15));

  Serial.print("Sensor 16(*C): ");
  Serial.println(sensors.getTempC(sensor16));

  Serial.print("Sensor 17(*C): ");
  Serial.println(sensors.getTempC(sensor17));

  Serial.print("Sensor 18(*C): ");
  Serial.println(sensors.getTempC(sensor18));

  Serial.print("Sensor 19(*C): ");
  Serial.println(sensors.getTempC(sensor19));

  Serial.print("Sensor 20(*C): ");
  Serial.println(sensors.getTempC(sensor20));

  Serial.print("Sensor 21(*C): ");
  Serial.println(sensors.getTempC(sensor21));

  Serial.print("Sensor 22(*C): ");
  Serial.println(sensors.getTempC(sensor22));

  Serial.print("Sensor 23(*C): ");
  Serial.println(sensors.getTempC(sensor23));

  Serial.print("Sensor 24(*C): ");
  Serial.println(sensors.getTempC(sensor24));


  String Temp1 = "Temperature : " + String(sensors.getTempC(sensor1)) + " °C";
  String Temp2 = "Temperature : " + String(sensors.getTempC(sensor2)) + " °C";
  String Temp3 = "Temperature : " + String(sensors.getTempC(sensor3)) + " °C";
  String Temp4 = "Temperature : " + String(sensors.getTempC(sensor4)) + " °C";
  String Temp5 = "Temperature : " + String(sensors.getTempC(sensor5)) + " °C";
  String Temp6 = "Temperature : " + String(sensors.getTempC(sensor6)) + " °C";
  String Temp7 = "Temperature : " + String(sensors.getTempC(sensor7)) + " °C";
  String Temp8 = "Temperature : " + String(sensors.getTempC(sensor8)) + " °C";
  String Temp9 = "Temperature : " + String(sensors.getTempC(sensor9)) + " °C";
  String Temp10 = "Temperature : " + String(sensors.getTempC(sensor10)) + " °C";
  String Temp11 = "Temperature : " + String(sensors.getTempC(sensor11)) + " °C";
  String Temp12 = "Temperature : " + String(sensors.getTempC(sensor12)) + " °C";
  String Temp13 = "Temperature : " + String(sensors.getTempC(sensor13)) + " °C";
  String Temp14 = "Temperature : " + String(sensors.getTempC(sensor14)) + " °C";
  String Temp15 = "Temperature : " + String(sensors.getTempC(sensor15)) + " °C";
  String Temp16 = "Temperature : " + String(sensors.getTempC(sensor16)) + " °C";
  String Temp17 = "Temperature : " + String(sensors.getTempC(sensor17)) + " °C";
  String Temp18 = "Temperature : " + String(sensors.getTempC(sensor18)) + " °C";
  String Temp19 = "Temperature : " + String(sensors.getTempC(sensor19)) + " °C";
  String Temp20 = "Temperature : " + String(sensors.getTempC(sensor20)) + " °C";
  String Temp21 = "Temperature : " + String(sensors.getTempC(sensor21)) + " °C";
  String Temp22 = "Temperature : " + String(sensors.getTempC(sensor22)) + " °C";
  String Temp23 = "Temperature : " + String(sensors.getTempC(sensor23)) + " °C";
  String Temp24 = "Temperature : " + String(sensors.getTempC(sensor24)) + " °C";

  sendData(sensors.getTempC(sensor1), sensors.getTempC(sensor2), sensors.getTempC(sensor3), sensors.getTempC(sensor4), sensors.getTempC(sensor5), sensors.getTempC(sensor6), sensors.getTempC(sensor7), sensors.getTempC(sensor8), sensors.getTempC(sensor9), sensors.getTempC(sensor10), sensors.getTempC(sensor11), sensors.getTempC(sensor12), sensors.getTempC(sensor13), sensors.getTempC(sensor14), sensors.getTempC(sensor15), sensors.getTempC(sensor16), sensors.getTempC(sensor17), sensors.getTempC(sensor18), sensors.getTempC(sensor19), sensors.getTempC(sensor20), sensors.getTempC(sensor21), sensors.getTempC(sensor22), sensors.getTempC(sensor23), sensors.getTempC(sensor24));
  delay(2000);
}

// Subroutine for sending data to Google Sheets
void sendData(float tem1, float tem2, float tem3, float tem4, float tem5, float tem6, float tem7, float tem8, float tem9, float tem10, float tem11 , float tem12, float tem13, float tem14, float tem15, float tem16, float tem17, float tem18, float tem19, float tem20, float tem21, float tem22, float tem23, float tem24) {
  Serial.println("==========");
  Serial.print("connecting to ");
  Serial.println(host);

  //----------------------------------------Connect to Google host
  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }
  //----------------------------------------

  //----------------------------------------Processing data and sending data
  String string_temperature1 =  String(tem1);
  String string_temperature2 =  String(tem2);
  String string_temperature3 =  String(tem3);
  String string_temperature4 =  String(tem4);
  String string_temperature5 =  String(tem5);
  String string_temperature6 =  String(tem6);
  String string_temperature7 =  String(tem7);
  String string_temperature8 =  String(tem8);
  String string_temperature9 =  String(tem9);
  String string_temperature10 =  String(tem10);
  String string_temperature11 =  String(tem11);
  String string_temperature12 =  String(tem12);
  String string_temperature13 =  String(tem13);
  String string_temperature14 =  String(tem14);
  String string_temperature15 =  String(tem15);
  String string_temperature16 =  String(tem16);
  String string_temperature17 =  String(tem17);
  String string_temperature18 =  String(tem18);
  String string_temperature19 =  String(tem19);
  String string_temperature20 =  String(tem20);
  String string_temperature21 =  String(tem21);
  String string_temperature22 =  String(tem22);
  String string_temperature23 =  String(tem23);
  String string_temperature24 =  String(tem24);

  String url = "/macros/s/" + GAS_ID + "/exec?temperature1=" + string_temperature1 + "&temperature2=" + string_temperature2 + "&temperature3=" + string_temperature3 + "&temperature4=" + string_temperature4 + "&temperature5=" + string_temperature5 + "&temperature6=" + string_temperature6 + "&temperature7=" + string_temperature7 + "&temperature8=" + string_temperature8 + "&temperature9=" + string_temperature9 + "&temperature10=" + string_temperature10 + "&temperature11=" + string_temperature11 + "&temperature12=" + string_temperature12 + "&temperature13=" + string_temperature13 + "&temperature14=" + string_temperature14 + "&temperature15=" + string_temperature15 + "&temperature16=" + string_temperature16 + "&temperature17=" + string_temperature17 + "&temperature18=" + string_temperature18 + "&temperature19=" + string_temperature19 + "&temperature20=" + string_temperature20 + "&temperature21=" + string_temperature21 + "&temperature22=" + string_temperature22 + "&temperature23=" + string_temperature23 + "&temperature24=" + string_temperature24;
  Serial.print("requesting URL: ");
  Serial.println(url);

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: BuildFailureDetectorESP8266\r\n" +
               "Connection: close\r\n\r\n");

  Serial.println("request sent");

  //----------------------------------------Checking whether the data was sent successfully or not
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("headers received");
      break;
    }
  }
  String line = client.readStringUntil('\n');
  if (line.startsWith("{\"state\":\"success\"")) {
    Serial.println("esp8266/Arduino CI successfull!");
  } else {
    Serial.println("esp8266/Arduino CI has failed");
  }
  Serial.print("reply was : ");
  Serial.println(line);
  Serial.println("closing connection");
  Serial.println("==========");
  Serial.println();
  //----------------------------------------
}
//==============================================================================
