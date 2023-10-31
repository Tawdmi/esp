#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

#define LDR_PIN A0
#define BUTTON1_PIN 2
#define BUTTON2_PIN 3
#define LED1_PIN 4
#define LED2_PIN 5

LiquidCrystal_I2C lcd(0x27,16,2); 

void setup() {
  pinMode(LDR_PIN, INPUT);
  pinMode(BUTTON1_PIN, INPUT_PULLUP);
  pinMode(BUTTON2_PIN, INPUT_PULLUP);
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);

  lcd.init();
  lcd.backlight();

  Serial.begin(9600);
}

void loop() {
  int ldrValue = analogRead(LDR_PIN);
  int button1State = digitalRead(BUTTON1_PIN);
  int button2State = digitalRead(BUTTON2_PIN);

  if (ldrValue < 400) {
    digitalWrite(LED1_PIN, LOW);
    digitalWrite(LED2_PIN, LOW);
  } else if (ldrValue >= 400 && ldrValue <= 600) {
    digitalWrite(LED1_PIN, HIGH);
    digitalWrite(LED2_PIN, LOW);
  } else if (ldrValue > 600) {
    digitalWrite(LED1_PIN, LOW);
    digitalWrite(LED2_PIN, HIGH);
  }

  if (button1State == LOW) {
    digitalWrite(LED1_PIN, !digitalRead(LED1_PIN));
    delay(200);
  }

  if (button2State == LOW) {
    digitalWrite(LED2_PIN, !digitalRead(LED2_PIN));
    delay(200);
  }

  Serial.println(ldrValue);

  lcd.setCursor(0,0);
  lcd.print("LDR Value: ");
  lcd.print(ldrValue);

}
