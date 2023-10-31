    // Fill-in information from your Blynk Template here blynh mail truongnatson@gmail.com
#define BLYNK_TEMPLATE_ID "TMPL6QLsXCxOG"
#define BLYNK_DEVICE_NAME "Ao cá thông minh"

#define BLYNK_FIRMWARE_VERSION        "0.1.0"
#define BLYNK_PRINT Serial
//#define BLYNK_DEBUG

#define APP_DEBUG
#define USE_NODE_MCU_BOARD
#include "BlynkEdgent.h"
#include "DHT.h"
#define DHTPIN 5 //cam bien out chan d1
#define DHTTYPE DHT11
#define led1 14 //chan D5
#define led2 12 //D6
#define led3 13 //D7

#define button1 0 //D3
#define button2 4 //D2
#define button3 2 //D4

DHT dht(DHTPIN, DHTTYPE);
boolean bt1_state=HIGH;
boolean bt2_state=HIGH;
boolean bt3_state=HIGH;
unsigned long times=millis();

WidgetLED led(V5);
BlynkTimer timer;

// V5 LED Widget is blinking
void blinkLedWidget(){
  if (led.getValue()) {
    led.off();
  } else {
    led.on();
  }
}

void setup(){
  Serial.begin(115200);
  delay(100);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);

  pinMode(button1,INPUT_PULLUP);
  pinMode(button2,INPUT_PULLUP);
  pinMode(button3,INPUT_PULLUP);

  BlynkEdgent.begin();
  dht.begin();
  timer.setInterval(1000L, blinkLedWidget);
}

void loop() {
  BlynkEdgent.run();
  timer.run();
  if(millis()-times>2000){
    // Reading temperature or humidity
    float t = dht.readTemperature();
    float h = dht.readHumidity();
    if (isnan(h) || isnan(t)) {
      Serial.println(F("Failed to read from DHT sensor!"));
      return;
    }
    Blynk.virtualWrite(V4,t);
    Blynk.virtualWrite(V0,h);
    Serial.print(F("Temperature: "));
    Serial.print(t);
    Serial.print(F("°C "));
    Serial.print(F("Humidity: "));
    Serial.print(h);
    Serial.println(F("%"));
    times = millis();
  }
   check_button();
}
BLYNK_WRITE(V1){
  int p = param.asInt();
  digitalWrite(led1, p); 
}
BLYNK_WRITE(V2){
  int p = param.asInt();
  digitalWrite(led2, p); 
}
BLYNK_WRITE(V3){
  int p = param.asInt();
  digitalWrite(led3, p); 
}
void check_button(){
  if(digitalRead(button1)==LOW){
    if(bt1_state==HIGH){
      digitalWrite(led1,!digitalRead(led1));
      Blynk.virtualWrite(V1,digitalRead(led1));
      bt1_state=LOW;
      delay(200);
    }
  }else{
    bt1_state=HIGH;
  }
  if(digitalRead(button2)==LOW){
    if(bt2_state==HIGH){
      digitalWrite(led2,!digitalRead(led2));
      Blynk.virtualWrite(V2,digitalRead(led2));
      bt2_state=LOW;
      delay(200);
    }
  }else{
    bt2_state=HIGH;
  }
  
   if(digitalRead(button3)==LOW){
    if(bt3_state==HIGH){
      digitalWrite(led3,!digitalRead(led3));
      Blynk.virtualWrite(V3,digitalRead(led3));
      bt3_state=LOW;
      delay(200);
    }
  }else{
    bt2_state=HIGH;
  }
}
