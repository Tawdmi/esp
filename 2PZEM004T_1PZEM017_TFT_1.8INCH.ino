// un-comment this line to print the debugging statements
#define DEBUG
#include "espConfig.h"

#include <ModbusMaster.h>                   // Load the (modified) library for modbus communication command codes. Kindly install at our website.
#define MAX485_DE      23                    // Define DE Pin to Arduino pin. Connect DE Pin of Max485 converter module to Pin 2 (default) Arduino board
#define MAX485_RE      23                    // Define RE Pin to Arduino pin. Connect RE Pin of Max485 converter module to Pin 3 (default) Arduino board
// These DE anr RE pins can be any other Digital Pins to be activated during transmission and reception process.
static uint8_t pzemSlaveAddr = 0x01;        // Declare the address of device (meter) in term of 8 bits. You can change to 0x02 etc if you have more than 1 meter.
static uint16_t NewshuntAddr = 0x0002;      // Declare your external shunt value. Default is 100A, replace to "0x0001" if using 50A shunt, 0x0002 is for 200A, 0x0003 is for 300A
// By default manufacturer may already set, however, to set manually kindly delete the "//" for line "// setShunt(0x01);" in void setup
ModbusMaster node;                          /* activate modbus master codes*/
float PZEMVoltage = 0;                      /* Declare value for DC voltage */
float PZEMCurrent = 0;                      /* Declare value for DC current*/
float PZEMPower = 0;                        /* Declare value for DC Power */
float PZEMEnergy = 0;                       /* Declare value for DC Energy */
unsigned long startMillisPZEM = millis();
unsigned long currentMillisPZEM = millis();
#define periodPZEM 2000

#include <PZEM004Tv30.h>
//#include <SoftwareSerial.h>
#define PZEM_RX_PIN 16
#define PZEM_TX_PIN 17
//SoftwareSerial pzemSWSerial(PZEM_RX_PIN, PZEM_TX_PIN);
#define NUM_PZEMS 3 //số lượng pzem đang dùng gồm 1zpzem017 và 2pzem004T
PZEM004Tv30 pzems[NUM_PZEMS];
#include <TimeLib.h>
#include <WidgetRTC.h>
float csHomqua[NUM_PZEMS], csHomtruoc[NUM_PZEMS], csThangroi[NUM_PZEMS], csThangtruoc[NUM_PZEMS], tongDiennang[NUM_PZEMS];
int dayWrite = 1;
int hourWrite = 23, minuteWrite = 59;
WidgetRTC rtc;  //Khai báo sử dụng thời gian thực
BlynkTimer timer;
boolean clearState = 0;
unsigned long pushTime = millis();
unsigned long timeUp = millis();
WidgetLED led(V0);
#define U0     V1
#define I0     V2
#define P0     V3
#define E0     V6
#define DNHomNay0       V7
#define DNHomQua0       V8
#define DNThangNay0     V9
#define DNThangTruoc0   V10
#define U1     V11
#define I1     V12
#define P1     V13
#define F1     V14
#define PF1    V15
#define E1     V16
#define DNHomNay1       V17
#define DNHomQua1       V18
#define DNThangNay1     V19
#define DNThangTruoc1   V20
#define U2     V21
#define I2     V22
#define P2     V23
#define F2     V24
#define PF2    V25
#define E2     V26
#define DNHomNay2       V27
#define DNHomQua2       V28
#define DNThangNay2     V29
#define DNThangTruoc2   V30
#define DayWrite     V31
#define Reset        V32
WidgetLED LedReset  (V33);
#define Uptime       V34
#define OUT1         V35

char uptimesString[15];

//Phần hiển thị TFT
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>

#define TFT_CS       5  //      
#define TFT_RST      18   //                                          
#define TFT_DC       19   //      
#define TFT_MOSI     21   //      
#define TFT_SCLK     3  //     
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);
#define BLACK 0x0000
#define BLUE 0x001F
#define RED 0xF800
#define GREEN 0x07E0
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0
#define WHITE 0xFFFF

//Phần cảm biến LM35
#include <Wire.h>
const int lm35Pin1 = 34; // LM35 num 1 is connected to GPIO 34
const int lm35Pin2 = 35; // LM35 num 2 is connected to GPIO 35

const int fanPin = 22; // Fan is connected to GPIO 5
const float temperatureThreshold = 50.0; // Temperature threshold in Celsius
const int fanSpeed = 128; // Fan speed (0-255)

// Use channel 0 for the fan
const int fanChannel = 0;
// Use a frequency of 5000 Hz
const int fanFreq = 5000;
// Use 8 bits for resolution
const int fanResolution = 8;

const int btResetAll = 12;
const int out1       = 27;
const int btOut1     = 14;
boolean btOut1State = 0;
unsigned long push_btResetAll = millis();
unsigned long timeDelay = millis();

void setup() {
  Serial.begin(115200);
  pinMode(btResetAll, INPUT_PULLUP);
  pinMode(btOut1, INPUT_PULLUP);
  pinMode(out1, OUTPUT);

  // Configure LED PWM functionalitites
  ledcSetup(fanChannel, fanFreq, fanResolution);

  // Attach the channel to the GPIO pin
  ledcAttachPin(fanPin, fanChannel);

  //digitalWrite(out1,HIGH);
  attachInterrupt(btResetAll, handle_btResetAll, FALLING);
  attachInterrupt(btOut1, handle_btOut1, FALLING);
  for (int i = 1; i < NUM_PZEMS; i++) {
    pzems[i] = PZEM004Tv30(Serial2, PZEM_RX_PIN, PZEM_TX_PIN, 0x10 + i);
  }
  espConfig.begin();

  readChiSo();
  setSyncInterval(60 * 60);
  timer.setInterval(60000L, writeChiSo); //timer kiểm tra ghi số điện

  //Serial2.begin(9600,SERIAL_8N2);             /* To assign communication port to communicate with meter. with 2 stop bits (refer to manual)*/
  // By default communicate via Serial2 port: pin 14 (Tx) and pin 15 (Rx)
  node.begin(pzemSlaveAddr, Serial2);         /* Define and start the Modbus RTU communication. Communication to specific slave address and which Serial port */
  pinMode(MAX485_RE, OUTPUT);                 /* Define RE Pin as Signal Output for RS485 converter. Output pin means Arduino command the pin signal to go high or low so that signal is received by the converter*/
  pinMode(MAX485_DE, OUTPUT);                 /* Define DE Pin as Signal Output for RS485 converter. Output pin means Arduino command the pin signal to go high or low so that signal is received by the converter*/
  digitalWrite(MAX485_RE, 0);                 /* Arduino create output signal for pin RE as LOW (no output)*/
  digitalWrite(MAX485_DE, 0);                 /* Arduino create output signal for pin DE as LOW (no output)*/
  // both pins no output means the converter is in communication signal receiving mode
  node.preTransmission(preTransmission);      // Callbacks allow us to configure the RS485 transceiver correctly
  node.postTransmission(postTransmission);
  // setShunt(pzemSlaveAddr);                          // Delete the "//" to set shunt rating (0x01) is the meter address by default
  // changeAddress(0XF8, 0x01);                  // By delete the double slash symbol, the meter address will be set as 0x01
  setupLCD();
}

void loop() {
  espConfig.run();
  app_loop();
  fancontrol();
}

void fancontrol() {
  float voltage1 = analogRead(lm35Pin1) * (3.3 / 4096.0);  //doc nhiet do lm35 so 1
  float temperature1 = voltage1 * 100;
  if (temperature1 >= temperatureThreshold) {
    // Turn on the fan at the configured speed
    ledcWrite(fanChannel, fanSpeed);
  } else {
    // Turn off the fan
    ledcWrite(fanChannel, 0);
  }
  delay(1000);
}

void handle_btResetAll() {
  push_btResetAll = millis();
  Serial.println("Push 15s for Reset defaut!");
}
void handle_btOut1() {
  if (millis() - timeDelay > 500) {
    digitalWrite(out1, !digitalRead(out1));
    Serial.println("Output: " + String(digitalRead(out1)));
    btOut1State = 1;
    timeDelay = millis();
  }
}
void app_loop() {
  handleData();
  int uptimeSd = int((millis() / 1000) % 60);
  int uptimeMn = int((millis() / (1000 * 60)) % 60);
  int uptimeHr = int((millis() / (1000 * 60 * 60)) % 24);
  int uptimeDy = int((millis() / (1000 * 60 * 60 * 24)) % 365);
  sprintf(uptimesString, "%2dd:%2dh:%2dm:%2ds", uptimeDy, uptimeHr, uptimeMn, uptimeSd);

  if (digitalRead(btResetAll) == LOW) {
    if (millis() - push_btResetAll > 15000) {
      resetPzem017();
      for (int n = 1; n < NUM_PZEMS; n++) {
        pzems[n].resetEnergy();
      }
      for (int i = 0; i < 512; ++i) {
        EEPROM.write(i, 0);
        delay(10);
      }
      EEPROM.commit();
      Serial.println("Reset all..............!");
      delay(2000);
      ESP.restart();
      push_btResetAll = millis();
    }
  } else {
    push_btResetAll = millis();
  }
  if (btOut1State == 1) {
    Blynk.virtualWrite(OUT1, digitalRead(out1));
    btOut1State = 0;
  }
}
void handleData() {
  timer.run();
  if (clearState == 1) {
    if (millis() - pushTime > 5000) {
      LedReset.on();
      resetPzem017();
      for (int n = 1; n < NUM_PZEMS; n++) {
        pzems[n].resetEnergy();
      }
      for (int i = 0; i < 16 * NUM_PZEMS; ++i) {
        EEPROM.write(i, 0);
        delay(10);
      }
      EEPROM.commit();
      readChiSo();
      LedReset.off();
      pushTime = millis();
    }
  }
  if (millis() - timeUp > 3000) {
    if (led.getValue()) {
      led.off();
    } else {
      led.on();
    }
    //----------pzem017-------------------------------------------
    currentMillisPZEM = millis();                                                                     /* count time for program run every second (by default)*/
    if (currentMillisPZEM - startMillisPZEM >= periodPZEM)                                            /* for every x seconds, run the codes below*/
    {
      uint8_t result;                                                                                 /* Declare variable "result" as 8 bits */
      result = node.readInputRegisters(0x0000, 6);                                                    /* read the 9 registers (information) of the PZEM-014 / 016 starting 0x0000 (voltage information) kindly refer to manual)*/
      if (result == node.ku8MBSuccess)                                                                /* If there is a response */
      {
        uint32_t tempdouble = 0x00000000;                                                           /* Declare variable "tempdouble" as 32 bits with initial value is 0 */
        PZEMVoltage = node.getResponseBuffer(0x0000) / 100.0;                                       /* get the 16bit value for the voltage value, divide it by 100 (as per manual) */
        // 0x0000 to 0x0008 are the register address of the measurement value
        PZEMCurrent = node.getResponseBuffer(0x0001) / 100.0;                                       /* get the 16bit value for the current value, divide it by 100 (as per manual) */

        tempdouble =  (node.getResponseBuffer(0x0003) << 16) + node.getResponseBuffer(0x0002);      /* get the power value. Power value is consists of 2 parts (2 digits of 16 bits in front and 2 digits of 16 bits at the back) and combine them to an unsigned 32bit */
        PZEMPower = tempdouble / 10.0;                                                              /* Divide the value by 10 to get actual power value (as per manual) */

        tempdouble =  (node.getResponseBuffer(0x0005) << 16) + node.getResponseBuffer(0x0004);      /* get the energy value. Energy value is consists of 2 parts (2 digits of 16 bits in front and 2 digits of 16 bits at the back) and combine them to an unsigned 32bit */
        PZEMEnergy = tempdouble;

        Serial.print(PZEMVoltage, 1);                                                               /* Print Voltage value on Serial Monitor with 1 decimal*/
        Serial.print("V   ");
        Serial.print(PZEMCurrent, 3);
        Serial.print("A   ");
        Serial.print(PZEMPower, 1);
        Serial.print("W  ");
        Serial.print(PZEMEnergy, 0);
        Serial.print("Wh  ");
        Serial.println();

        tongDiennang[0] = PZEMEnergy;

        Blynk.virtualWrite(U0, PZEMVoltage);
        Blynk.virtualWrite(I0, PZEMCurrent);
        Blynk.virtualWrite(P0, PZEMPower);
        Blynk.virtualWrite(E0, PZEMEnergy);

        Blynk.virtualWrite(DNHomNay0, tongDiennang[0] - csHomqua[0]);
        Blynk.virtualWrite(DNHomQua0, csHomqua[0] - csHomtruoc[0]);
        Blynk.virtualWrite(DNThangNay0, tongDiennang[0] - csThangroi[0]);
        Blynk.virtualWrite(DNThangTruoc0, csThangroi[0] - csThangtruoc[0]);

        showLCD(0, PZEMVoltage, PZEMCurrent, PZEMPower, 0, 0, PZEMEnergy);
      }
      else
      {
        Serial.println("Failed to read modbus");
        showLCD(0, 0, 0, 0, 0, 0, 0);
      }
      startMillisPZEM = currentMillisPZEM ;                                                       /* Set the starting point again for next counting time */
    }
    // //-------------pzem004t-------------------------------
    for (int i = 1; i < NUM_PZEMS; i++) {
      Serial.print("PZEM ");
      Serial.print(i);
      Serial.print(" - Address:");
      Serial.println(pzems[i].getAddress(), HEX);
      Serial.println("===================");

      float voltage = pzems[i].voltage();
      float current = pzems[i].current();
      float power = pzems[i].power();
      float energy = pzems[i].energy();
      float frequency = pzems[i].frequency();
      float pf = pzems[i].pf();
      if (isnan(voltage)) {
        Serial.println("Error reading voltage");
        showLCD(i, 0, 0, 0, 0, 0, 0);
      } else if (isnan(current)) {
        Serial.println("Error reading current");
      } else if (isnan(power)) {
        Serial.println("Error reading power");
      } else if (isnan(energy)) {
        Serial.println("Error reading energy");
      } else if (isnan(frequency)) {
        Serial.println("Error reading frequency");
      } else if (isnan(pf)) {
        Serial.println("Error reading power factor");
      } else {
        // Serial.print("Voltage: ");      Serial.print(voltage);      Serial.println("V");
        // Serial.print("Current: ");      Serial.print(current);      Serial.println("A");
        // Serial.print("Power: ");        Serial.print(power);        Serial.println("W");
        // Serial.print("Energy: ");       Serial.print(energy,3);     Serial.println("kWh");
        // Serial.print("Frequency: ");    Serial.print(frequency, 1); Serial.println("Hz");
        // Serial.print("PF: ");           Serial.println(pf);

        tongDiennang[i] = energy;
        showLCD(i, voltage, current, power, frequency, pf, energy);
        if (i == 1) {
          Blynk.virtualWrite(U1, voltage);
          Blynk.virtualWrite(I1, current);
          Blynk.virtualWrite(P1, power);
          Blynk.virtualWrite(F1, frequency);
          Blynk.virtualWrite(PF1, pf);
          Blynk.virtualWrite(E1, energy);

          Blynk.virtualWrite(DNHomNay1, tongDiennang[i] - csHomqua[i]);
          Blynk.virtualWrite(DNHomQua1, csHomqua[i] - csHomtruoc[i]);
          Blynk.virtualWrite(DNThangNay1, tongDiennang[i] - csThangroi[i]);
          Blynk.virtualWrite(DNThangTruoc1, csThangroi[i] - csThangtruoc[i]);
        } else if (i == 2) {
          Blynk.virtualWrite(U2, voltage);
          Blynk.virtualWrite(I2, current);
          Blynk.virtualWrite(P2, power);
          Blynk.virtualWrite(F2, frequency);
          Blynk.virtualWrite(PF2, pf);
          Blynk.virtualWrite(E2, energy);

          Blynk.virtualWrite(DNHomNay2, tongDiennang[i] - csHomqua[i]);
          Blynk.virtualWrite(DNHomQua2, csHomqua[i] - csHomtruoc[i]);
          Blynk.virtualWrite(DNThangNay2, tongDiennang[i] - csThangroi[i]);
          Blynk.virtualWrite(DNThangTruoc2, csThangroi[i] - csThangtruoc[i]);
        }
      }
      //delay(1000);
    }
    Blynk.virtualWrite(Uptime, uptimesString);
    timeUp = millis();
  }
}
//------------------TFT------------------------------
void setupLCD() {
  tft.init(240, 320);                        //Thiet lap LCD TFT
  tft.fillScreen(BLACK);                          //Thiet lap mau nen LCD
  tft.setRotation(3);
  tft.drawRect(0, 0, 315, 230, MAGENTA);          //Hien thi khung
  tft.drawRect(1, 1, 314, 229, GREEN);

  //tft.drawLine(2, 10, 156, 10, WHITE);

  printText("DC", MAGENTA, 15, 15, 2);
  printText("AC1", CYAN, 120, 15, 3);
  printText("AC2", YELLOW, 225, 15, 3);

  tft.drawLine(2, 35, 312, 35, WHITE);

  //tft.drawLine(35, 35, 35, 240, WHITE);

  tft.drawLine(2 , 175, 312, 175, WHITE);

  tft.drawFastVLine(99, 2, 222  , WHITE);
  tft.drawFastVLine(205, 2, 222, WHITE);

  //printText("U(V)",RED,5,45,1);
  printText("300.0V", RED, 10, 45, 7);
  printText("260.0V", RED, 110, 45, 7);
  printText("260.0V", RED, 215, 45, 7);

  // ngang x doc x ky tu

  //printText("I(A)",BLUE,5,95,1);
  printText("100.0A", BLUE, 10, 95, 7);
  printText("100.0A", BLUE, 110, 95, 7);
  printText("100.0A", BLUE, 215, 95, 7);

  // ngang x doc x ky tu

  //printText("P(W)",GREEN,5,120,1);
  //printText("10000W",GREEN,10,120,7);
  printText("10000W", GREEN, 110, 120, 7);
  printText("10000W", GREEN, 215, 120, 7);

  // ngang x doc x ky tu

  //printText("F(Hz)",CYAN,5,145,1);
  printText("100°C", RED, 10, 145, 7);
  printText("60.0Hz", CYAN, 110, 145, 7);
  printText("60.0Hz", CYAN, 215, 145, 7);

  // ngang x doc x ky tu

  //printText("PF()",WHITE,10,190,1);
  printText("100°C", RED, 10, 190, 7);
  printText("0.99PF", WHITE, 110, 190, 7);
  printText("0.99PF", WHITE, 215, 190, 7);

  // ngang x doc x ky tu

  //printText("E kWh",YELLOW,5,190,1);
  printText("9999kWh", YELLOW, 10, 190, 7);
  printText("9999kWh", YELLOW, 110, 190, 7);
  printText("9999kWh", YELLOW, 215, 190, 7);

  // ngang x doc x số ký tự
}
void showLCD(int n, float u, float i, float p, float f, float pf, float e) {

  float voltage1 = analogRead(lm35Pin1) * (3.3 / 4096.0);  //doc nhiet do lm35 so 1
  float temperature1 = voltage1 * 100;
  float voltage2 = analogRead(lm35Pin2) * (3.3 / 4096.0);  //doc nhiẹt do lm35 so 2
  float temperature2 = voltage2 * 100;

  if (n == 0) { // cột 1  ngang 5 doc 50 70 100

    printText(String(u, 1) + "V", RED, 10, 45, 7);
    printText(String(i, 1) + "A", BLUE, 10, 70, 7);
    printText(String(p, 0) + "W", GREEN, 10, 95, 7);
    printText(String(voltage1, 2) + "°C", RED, 10, 120, 7); //hien thi nhiet do 1
    printText(String(voltage2, 2) + "°C", RED, 10, 145, 7); //hien thi nhiet do 2
    printText(String(e, 0) + "Wh", YELLOW, 10, 190, 7);


  } else if (n == 1) { //cột 2 ngang 180 doc 50 70 100

    printText(String(u, 1) + "V", RED, 110, 45, 7);
    printText(String(i, 1) + "A", BLUE, 110, 70, 7);
    printText(String(p, 0) + "W", GREEN, 110, 95, 7);
    printText(String(f, 1) + "Hz", CYAN, 110, 120, 7);
    printText(String(pf, 2) + "PF", WHITE, 110, 145, 7);
    printText(String(e, 0) + "kWh", YELLOW, 110, 190, 7);

  } else if (n == 2) { //cột 3 ngang 205 doc 180

    printText(String(u, 1) + "V", RED, 215, 45, 7);
    printText(String(i, 1) + "A", BLUE, 215, 70, 7);
    printText(String(p, 0) + "W", GREEN, 215, 95, 7);
    printText(String(f, 1) + "Hz", CYAN, 215, 120, 7);
    printText(String(pf, 2) + "PF", WHITE, 215, 145, 7);
    printText(String(e, 0) + "kWh", YELLOW, 215 , 190, 7);
    // ngang x doc x ky tu
  }
}
void printText(String text, uint16_t color, int x, int y, int numchar) {
  tft.setCursor(x, y);
  tft.setTextColor(color, BLACK);
  tft.setTextSize(2);
  int sizetext = text.length();
  if (sizetext < numchar) {
    for (int i = (numchar - sizetext); i > 0; --i) {
      text = text + " ";
    }
  }
  tft.print(text);
}
void printText2(String text, uint16_t color, int x, int y, int numchar, int size) {
  tft.setCursor(x, y);
  tft.setTextColor(color, BLACK);
  tft.setTextSize(size);
  int sizetext = text.length();
  if (sizetext < numchar) {
    for (int i = (numchar - sizetext); i > 0; --i) {
      text = text + " ";
    }
  }
  tft.print(text);
}
//---------------------------------------------------------------------
void writeChiSo() {
  Serial.println("------>check time<--------" + String(hour()) + ":" + String(minute()));
  /*
     Timer kiểm tra thời gian để ghi số điện được kích hoặc 60s
     sau 60s sẽ gọi chương trình kiểm tra thời gian 1 lần
     nếu đúng giờ và phút chốt số điện của ngày thì ghi chỉ số ngày
     nếu đúng ngày chốt tháng thì ghi chỉ số tháng
  */
  if (hour() == hourWrite && minute() == minuteWrite) {
    Serial.println("Ghi số ngày mới!");
    for (int i = 0; i < NUM_PZEMS; i++) {
      writeFloat(4 + (16 * i), csHomqua[i]);
      writeFloat(0 + (16 * i), tongDiennang[i]);
      if (day() == dayWrite) {
        Serial.println("Ghi số tháng mới!");
        writeFloat(12 + (16 * i), csThangroi[i]);
        writeFloat(8 + (16 * i), tongDiennang[i]);
      }
    }
    EEPROM.commit();
    readChiSo();
  }
}
void readChiSo() {
  for (int i = 0; i < NUM_PZEMS; i++) {
    csHomqua[i] = readFloat(0 + (16 * i));
    csHomtruoc[i] = readFloat(4 + (16 * i));
    csThangroi[i] = readFloat(8 + (16 * i));
    csThangtruoc[i] = readFloat(12 + (16 * i));
    Serial.println();
    Serial.print("Chỉ số hôm qua    " + String(i) + ": "); Serial.println(csHomqua[i]);
    Serial.print("Chỉ số hôm trước  " + String(i) + ": "); Serial.println(csHomtruoc[i]);
    Serial.print("Chỉ số tháng rồi  " + String(i) + ": "); Serial.println(csThangroi[i]);
    Serial.print("Chỉ số tháng trước" + String(i) + ": "); Serial.println(csThangtruoc[i]);
  }
}
float readFloat(unsigned int addr) {
  union {
    byte b[4];
    float f;
  } data;
  for (int i = 0; i < 4; i++) {
    data.b[i] = EEPROM.read(addr + i);
  }
  return data.f;
}
void writeFloat(unsigned int addr, float x) {
  union {
    byte b[4];
    float f;
  } data;
  data.f = x;
  for (int i = 0; i < 4; i++) {
    EEPROM.write(addr + i, data.b[i]);
  }
}
//------------pzem017-----------
void resetPzem017() {
  // Reset energy

  uint16_t u16CRC = 0xFFFF;                                                                         /* declare CRC check 16 bits*/
  static uint8_t resetCommand = 0x42;                                                               /* reset command code*/
  uint8_t slaveAddr = pzemSlaveAddr;
  u16CRC = crc16_update(u16CRC, slaveAddr);
  u16CRC = crc16_update(u16CRC, resetCommand);
  Serial.println("Resetting Energy");
  preTransmission();                                                                                /* trigger transmission mode*/
  Serial2.write(slaveAddr);                                                                         /* send device address in 8 bit*/
  Serial2.write(resetCommand);                                                                      /* send reset command */
  Serial2.write(lowByte(u16CRC));                                                                   /* send CRC check code low byte  (1st part) */
  Serial2.write(highByte(u16CRC));                                                                  /* send CRC check code high byte (2nd part) */
  delay(10);
  postTransmission();                                                                               /* trigger reception mode*/
  delay(100);
  while (Serial2.available())                                                                       /* while receiving signal from Serial2 from meter and converter */
  {
    Serial.print(char(Serial2.read()), HEX);                                                      /* Prints the response and display on Serial Monitor (Serial)*/
    Serial.print(" ");
  }
}
void preTransmission()                                                                                    /* transmission program when triggered*/
{

  /* 1- PZEM-017 DC Energy Meter */

  digitalWrite(MAX485_RE, 1);                                                                       /* put RE Pin to high*/
  digitalWrite(MAX485_DE, 1);                                                                       /* put DE Pin to high*/
  delay(1);                                                                                         // When both RE and DE Pin are high, converter is allow to transmit communication
}

void postTransmission()                                                                                   /* Reception program when triggered*/
{

  /* 1- PZEM-017 DC Energy Meter */

  delay(3);                                                                                         // When both RE and DE Pin are low, converter is allow to receive communication
  digitalWrite(MAX485_RE, 0);                                                                       /* put RE Pin to low*/
  digitalWrite(MAX485_DE, 0);                                                                       /* put DE Pin to low*/
}


void setShunt(uint8_t slaveAddr)                                                                          //Change the slave address of a node
{

  /* 1- PZEM-017 DC Energy Meter */

  static uint8_t SlaveParameter = 0x06;                                                             /* Write command code to PZEM */
  static uint16_t registerAddress = 0x0003;                                                         /* change shunt register address command code */

  uint16_t u16CRC = 0xFFFF;                                                                         /* declare CRC check 16 bits*/
  u16CRC = crc16_update(u16CRC, slaveAddr);                                                         // Calculate the crc16 over the 6bytes to be send
  u16CRC = crc16_update(u16CRC, SlaveParameter);
  u16CRC = crc16_update(u16CRC, highByte(registerAddress));
  u16CRC = crc16_update(u16CRC, lowByte(registerAddress));
  u16CRC = crc16_update(u16CRC, highByte(NewshuntAddr));
  u16CRC = crc16_update(u16CRC, lowByte(NewshuntAddr));

  Serial.println("Change shunt address");
  preTransmission();                                                                                 /* trigger transmission mode*/

  Serial2.write(slaveAddr);                                                                       /* these whole process code sequence refer to manual*/
  Serial2.write(SlaveParameter);
  Serial2.write(highByte(registerAddress));
  Serial2.write(lowByte(registerAddress));
  Serial2.write(highByte(NewshuntAddr));
  Serial2.write(lowByte(NewshuntAddr));
  Serial2.write(lowByte(u16CRC));
  Serial2.write(highByte(u16CRC));
  delay(10);
  postTransmission();                                                                                /* trigger reception mode*/
  delay(100);
  while (Serial2.available())                                                                        /* while receiving signal from Serial2 from meter and converter */
  {
    Serial.print(char(Serial2.read()), HEX);                                                       /* Prints the response and display on Serial Monitor (Serial)*/
    Serial.print(" ");
  }
}

void changeAddress(uint8_t OldslaveAddr, uint8_t NewslaveAddr)                                            //Change the slave address of a node
{

  /* 1- PZEM-017 DC Energy Meter */

  static uint8_t SlaveParameter = 0x06;                                                             /* Write command code to PZEM */
  static uint16_t registerAddress = 0x0002;                                                         /* Modbus RTU device address command code */
  uint16_t u16CRC = 0xFFFF;                                                                         /* declare CRC check 16 bits*/
  u16CRC = crc16_update(u16CRC, OldslaveAddr);                                                      // Calculate the crc16 over the 6bytes to be send
  u16CRC = crc16_update(u16CRC, SlaveParameter);
  u16CRC = crc16_update(u16CRC, highByte(registerAddress));
  u16CRC = crc16_update(u16CRC, lowByte(registerAddress));
  u16CRC = crc16_update(u16CRC, highByte(NewslaveAddr));
  u16CRC = crc16_update(u16CRC, lowByte(NewslaveAddr));

  Serial.println("Change Slave Address");
  preTransmission();                                                                                 /* trigger transmission mode*/

  Serial2.write(OldslaveAddr);                                                                       /* these whole process code sequence refer to manual*/
  Serial2.write(SlaveParameter);
  Serial2.write(highByte(registerAddress));
  Serial2.write(lowByte(registerAddress));
  Serial2.write(highByte(NewslaveAddr));
  Serial2.write(lowByte(NewslaveAddr));
  Serial2.write(lowByte(u16CRC));
  Serial2.write(highByte(u16CRC));
  delay(10);
  postTransmission();                                                                                /* trigger reception mode*/
  delay(100);
  while (Serial2.available())                                                                        /* while receiving signal from Serial2 from meter and converter */
  {
    Serial.print(char(Serial2.read()), HEX);                                                       /* Prints the response and display on Serial Monitor (Serial)*/
    Serial.print(" ");
  }
}
//----------------------------------------------------------
BLYNK_CONNECTED() {
  rtc.begin();  //Khởi chạy hệ thống thời gian thực
  Blynk.syncVirtual(DayWrite); //Đồng bộ ngày chốt số điện cho tháng
  LedReset.off();
}
BLYNK_WRITE(DayWrite) {
  dayWrite = param.asInt(); //Lấy ngày chốt số điện từ blynk về
  Serial.println("Ngày chốt số: " + String(dayWrite));
}
BLYNK_WRITE(Reset) {
  clearState  = param.asInt();
  pushTime = millis();
}
BLYNK_WRITE(OUT1) {
  int p = param.asInt();
  digitalWrite(out1, p);
  Serial.println("Output: " + String(digitalRead(out1)));
}
