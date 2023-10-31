// un-comment this line to print the debugging statements
#define DEBUG
#include "espConfig.h"
#include <PZEM004Tv30.h>
//#include <SoftwareSerial.h>
#define PZEM_RX_PIN 16
#define PZEM_TX_PIN 17
//SoftwareSerial pzemSWSerial(PZEM_RX_PIN, PZEM_TX_PIN);
#define NUM_PZEMS 8
PZEM004Tv30 pzems[NUM_PZEMS];
#include <TimeLib.h>
#include <WidgetRTC.h>
float csHomqua[NUM_PZEMS], csHomtruoc[NUM_PZEMS], csThangroi[NUM_PZEMS], csThangtruoc[NUM_PZEMS], tongDiennang[NUM_PZEMS];
int dayWrite=1;
int hourWrite=23, minuteWrite=59;
WidgetRTC rtc;  //Khai báo sử dụng thời gian thực 
BlynkTimer timer;
boolean clearState=0;
unsigned long pushTime=millis();
unsigned long timeUp=millis();
WidgetLED led(V0);
#define U0     V1
#define I0     V2
#define P0     V3
#define F0     V4
#define PF0    V5
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
#define U3     V31
#define I3     V32
#define P3     V33
#define F3     V34
#define PF3    V35
#define E3     V36
#define DNHomNay3       V37
#define DNHomQua3       V38
#define DNThangNay3     V39
#define DNThangTruoc3   V40
#define U4     V41
#define I4     V42
#define P4     V43
#define F4     V44
#define PF4    V45
#define E4     V46
#define DNHomNay4       V47
#define DNHomQua4       V48
#define DNThangNay4     V49
#define DNThangTruoc4   V50
#define U5     V51
#define I5     V52
#define P5     V53
#define F5     V54
#define PF5    V55
#define E5     V56
#define DNHomNay5       V57
#define DNHomQua5       V58
#define DNThangNay5     V59
#define DNThangTruoc5   V60
#define U6     V61
#define I6     V62
#define P6     V63
#define F6     V64
#define PF6    V65
#define E6     V66
#define DNHomNay6       V67
#define DNHomQua6       V68
#define DNThangNay6     V69
#define DNThangTruoc6   V70
#define U7     V71
#define I7     V72
#define P7     V73
#define F7     V74
#define PF7    V75
#define E7     V76
#define DNHomNay7       V77
#define DNHomQua7       V78
#define DNThangNay7     V79
#define DNThangTruoc7   V80

#define TienThangNay0   V81
#define TienThangTruoc0 V82
#define TienThangNay1   V83
#define TienThangTruoc1 V84
#define TienThangNay2   V85
#define TienThangTruoc2 V86
#define TienThangNay3   V87
#define TienThangTruoc3 V88
#define TienThangNay4   V89
#define TienThangTruoc4 V90
#define TienThangNay5   V91
#define TienThangTruoc5 V92
#define TienThangNay6   V93
#define TienThangTruoc6 V94
#define TienThangNay7   V95
#define TienThangTruoc7 V96


#define DayWrite     V97
#define Reset        V98
WidgetLED LedReset(V99);
#define Uptime       V100

int mua1 = 1728;
int mua2 = 1786;
int mua3 = 2074;
int mua4 = 2612;
int mua5 = 2919;
int mua6 = 3015;

char uptimesString[15];
unsigned long timeHandleBill=millis();

void setup() {
  Serial.begin(115200);
  for(int i = 0; i < NUM_PZEMS; i++){
    pzems[i] = PZEM004Tv30(Serial2, PZEM_RX_PIN, PZEM_TX_PIN, 0x10 + i);
  }
  espConfig.begin();
  readChiSo();
  setSyncInterval(60*60);
  timer.setInterval(60000L,writeChiSo); //timer kiểm tra ghi số điện
}

void loop() {
  espConfig.run();
  app_loop();
  handleData();
}
void app_loop(){
  int uptimeSd = int((millis()/1000)%60);
  int uptimeMn = int((millis()/(1000*60))%60);
  int uptimeHr = int((millis()/(1000*60*60))%24);
  int uptimeDy = int((millis()/(1000*60*60*24))%365);
  sprintf(uptimesString,"%2dd:%2dh:%2dm:%2ds",uptimeDy,uptimeHr,uptimeMn,uptimeSd);
}
void handleData(){
  timer.run();
  if(clearState==1){
    if(millis()-pushTime>5000){
      LedReset.on();
      for(int n=0; n<NUM_PZEMS;n++){
       pzems[n].resetEnergy();
      }
      for (int i = 0; i < 16*NUM_PZEMS; ++i) {
        EEPROM.write(i, 0);           
        delay(10);
      }
      EEPROM.commit();
      readChiSo();
      LedReset.off();
      pushTime=millis();
    }
  }
  if(millis()-timeUp>3000){
    for(int i = 0; i < NUM_PZEMS; i++){
      if(led.getValue()){
        led.off();
      }else{
        led.on();
      }
      
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
      if(isnan(voltage)){
          Serial.println("Error reading voltage");
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

        tongDiennang[i]=energy;

        String tienThangnay="";
        String tienThangtruoc="";
        if(millis()-timeHandleBill>15000){
          float eThangnay = tongDiennang[i]-csThangroi[i];
          float eThangtruoc = csThangroi[i]-csThangtruoc[i];
          float tien_thangnay=0;
          float tien_thangtruoc=0;
          if(eThangnay>400){
            tien_thangnay = (eThangnay-400)*mua6 + 100*mua5 + 100*mua4 + 100*mua3 + 50*mua2 + 50*mua1;
          }else if(eThangnay>300){
            tien_thangnay = (eThangnay-300)*mua5 + 100*mua4 + 100*mua3 + 50*mua2 + 50*mua1;
          }else if(eThangnay>200){
            tien_thangnay = (eThangnay-200)*mua4 + 100*mua3 + 50*mua2 + 50*mua1;
          }else if(eThangnay>100){
            tien_thangnay = (eThangnay-100)*mua3 + 50*mua2 + 50*mua1;
          }else if(eThangnay>50){
            tien_thangnay = (eThangnay-50)*mua2  + 50*mua1;
          }else{
            tien_thangnay = eThangnay*mua1;
          }

          if(eThangtruoc>400){
            tien_thangtruoc = (eThangtruoc-400)*mua6 + 100*mua5 + 100*mua4 + 100*mua3 + 50*mua2 + 50*mua1;
          }else if(eThangtruoc>300){
            tien_thangtruoc = (eThangtruoc-300)*mua5 + 100*mua4 + 100*mua3 + 50*mua2 + 50*mua1;
          }else if(eThangtruoc>200){
            tien_thangtruoc = (eThangtruoc-200)*mua4 + 100*mua3 + 50*mua2 + 50*mua1;
          }else if(eThangtruoc>100){
            tien_thangtruoc = (eThangtruoc-100)*mua3 + 50*mua2 + 50*mua1;
          }else if(eThangtruoc>50){
            tien_thangtruoc = (eThangtruoc-50)*mua2  + 50*mua1;
          }else{
            tien_thangtruoc = eThangtruoc*mua1;
          }

          char buffer[14];
          long valTemp = tien_thangnay;
          tienThangnay = ultoa(valTemp, buffer);
          valTemp = tien_thangtruoc;
          tienThangtruoc = ultoa(valTemp, buffer);

          timeHandleBill=millis();
        }
        
        if(i==0){
          Blynk.virtualWrite(U0,voltage);
          Blynk.virtualWrite(I0,current);
          Blynk.virtualWrite(P0,power);
          Blynk.virtualWrite(F0,frequency);
          Blynk.virtualWrite(PF0,pf);
          Blynk.virtualWrite(E0,energy);

          Blynk.virtualWrite(DNHomNay0,tongDiennang[i]-csHomqua[i]);
          Blynk.virtualWrite(DNHomQua0,csHomqua[i]-csHomtruoc[i]);
          Blynk.virtualWrite(DNThangNay0,tongDiennang[i]-csThangroi[i]);
          Blynk.virtualWrite(DNThangTruoc0,csThangroi[i]-csThangtruoc[i]);

          Blynk.virtualWrite(TienThangNay0,tienThangnay);
          Blynk.virtualWrite(TienThangTruoc0,tienThangtruoc);
        }else if(i==1){
          Blynk.virtualWrite(U1,voltage);
          Blynk.virtualWrite(I1,current);
          Blynk.virtualWrite(P1,power);
          Blynk.virtualWrite(F1,frequency);
          Blynk.virtualWrite(PF1,pf);
          Blynk.virtualWrite(E1,energy);

          Blynk.virtualWrite(DNHomNay1,tongDiennang[i]-csHomqua[i]);
          Blynk.virtualWrite(DNHomQua1,csHomqua[i]-csHomtruoc[i]);
          Blynk.virtualWrite(DNThangNay1,tongDiennang[i]-csThangroi[i]);
          Blynk.virtualWrite(DNThangTruoc1,csThangroi[i]-csThangtruoc[i]);

          Blynk.virtualWrite(TienThangNay1,tienThangnay);
          Blynk.virtualWrite(TienThangTruoc1,tienThangtruoc);
        }else if(i==2){
          Blynk.virtualWrite(U2,voltage);
          Blynk.virtualWrite(I2,current);
          Blynk.virtualWrite(P2,power);
          Blynk.virtualWrite(F2,frequency);
          Blynk.virtualWrite(PF2,pf);
          Blynk.virtualWrite(E2,energy);

          Blynk.virtualWrite(DNHomNay2,tongDiennang[i]-csHomqua[i]);
          Blynk.virtualWrite(DNHomQua2,csHomqua[i]-csHomtruoc[i]);
          Blynk.virtualWrite(DNThangNay2,tongDiennang[i]-csThangroi[i]);
          Blynk.virtualWrite(DNThangTruoc2,csThangroi[i]-csThangtruoc[i]);

          Blynk.virtualWrite(TienThangNay2,tienThangnay);
          Blynk.virtualWrite(TienThangTruoc2,tienThangtruoc);
        }else if(i==3){
          Blynk.virtualWrite(U3,voltage);
          Blynk.virtualWrite(I3,current);
          Blynk.virtualWrite(P3,power);
          Blynk.virtualWrite(F3,frequency);
          Blynk.virtualWrite(PF3,pf);
          Blynk.virtualWrite(E3,energy);

          Blynk.virtualWrite(DNHomNay3,tongDiennang[i]-csHomqua[i]);
          Blynk.virtualWrite(DNHomQua3,csHomqua[i]-csHomtruoc[i]);
          Blynk.virtualWrite(DNThangNay3,tongDiennang[i]-csThangroi[i]);
          Blynk.virtualWrite(DNThangTruoc3,csThangroi[i]-csThangtruoc[i]);

          Blynk.virtualWrite(TienThangNay3,tienThangnay);
          Blynk.virtualWrite(TienThangTruoc3,tienThangtruoc);
        }else if(i==4){
          Blynk.virtualWrite(U4,voltage);
          Blynk.virtualWrite(I4,current);
          Blynk.virtualWrite(P4,power);
          Blynk.virtualWrite(F4,frequency);
          Blynk.virtualWrite(PF4,pf);
          Blynk.virtualWrite(E4,energy);

          Blynk.virtualWrite(DNHomNay4,tongDiennang[i]-csHomqua[i]);
          Blynk.virtualWrite(DNHomQua4,csHomqua[i]-csHomtruoc[i]);
          Blynk.virtualWrite(DNThangNay4,tongDiennang[i]-csThangroi[i]);
          Blynk.virtualWrite(DNThangTruoc4,csThangroi[i]-csThangtruoc[i]);

          Blynk.virtualWrite(TienThangNay4,tienThangnay);
          Blynk.virtualWrite(TienThangTruoc4,tienThangtruoc);
        }else if(i==5){
          Blynk.virtualWrite(U5,voltage);
          Blynk.virtualWrite(I5,current);
          Blynk.virtualWrite(P5,power);
          Blynk.virtualWrite(F5,frequency);
          Blynk.virtualWrite(PF5,pf);
          Blynk.virtualWrite(E5,energy);

          Blynk.virtualWrite(DNHomNay5,tongDiennang[i]-csHomqua[i]);
          Blynk.virtualWrite(DNHomQua5,csHomqua[i]-csHomtruoc[i]);
          Blynk.virtualWrite(DNThangNay5,tongDiennang[i]-csThangroi[i]);
          Blynk.virtualWrite(DNThangTruoc5,csThangroi[i]-csThangtruoc[i]);

          Blynk.virtualWrite(TienThangNay5,tienThangnay);
          Blynk.virtualWrite(TienThangTruoc5,tienThangtruoc);
        }else if(i==6){
          Blynk.virtualWrite(U6,voltage);
          Blynk.virtualWrite(I6,current);
          Blynk.virtualWrite(P6,power);
          Blynk.virtualWrite(F6,frequency);
          Blynk.virtualWrite(PF6,pf);
          Blynk.virtualWrite(E6,energy);

          Blynk.virtualWrite(DNHomNay6,tongDiennang[i]-csHomqua[i]);
          Blynk.virtualWrite(DNHomQua6,csHomqua[i]-csHomtruoc[i]);
          Blynk.virtualWrite(DNThangNay6,tongDiennang[i]-csThangroi[i]);
          Blynk.virtualWrite(DNThangTruoc6,csThangroi[i]-csThangtruoc[i]);

          Blynk.virtualWrite(TienThangNay6,tienThangnay);
          Blynk.virtualWrite(TienThangTruoc6,tienThangtruoc);
        }else if(i==7){
          Blynk.virtualWrite(U7,voltage);
          Blynk.virtualWrite(I7,current);
          Blynk.virtualWrite(P7,power);
          Blynk.virtualWrite(F7,frequency);
          Blynk.virtualWrite(PF7,pf);
          Blynk.virtualWrite(E7,energy);

          Blynk.virtualWrite(DNHomNay7,tongDiennang[i]-csHomqua[i]);
          Blynk.virtualWrite(DNHomQua7,csHomqua[i]-csHomtruoc[i]);
          Blynk.virtualWrite(DNThangNay7,tongDiennang[i]-csThangroi[i]);
          Blynk.virtualWrite(DNThangTruoc7,csThangroi[i]-csThangtruoc[i]);

          Blynk.virtualWrite(TienThangNay7,tienThangnay);
          Blynk.virtualWrite(TienThangTruoc7,tienThangtruoc);
        }
      }
      delay(300);
    }
    Blynk.virtualWrite(Uptime,uptimesString);
    timeUp=millis();
  }
}
char *ultoa(unsigned long val, char *s){
    char *p = s + 13;
    *p = '\0';
    do {
        if ((p - s) % 4 == 2)
            *--p = ',';
        *--p = '0' + val % 10;
        val /= 10;
    } while (val);
    return p;
}
void writeChiSo(){
  Serial.println("------>check time<--------"+String(hour())+":"+String(minute()));
 /*
  * Timer kiểm tra thời gian để ghi số điện được kích hoặc 60s
  * sau 60s sẽ gọi chương trình kiểm tra thời gian 1 lần
  * nếu đúng giờ và phút chốt số điện của ngày thì ghi chỉ số ngày
  * nếu đúng ngày chốt tháng thì ghi chỉ số tháng
  */
  if(hour()==hourWrite && minute() == minuteWrite){
    Serial.println("Ghi số ngày mới!");
    for(int i=0;i<NUM_PZEMS;i++){
      writeFloat(4+(16*i),csHomqua[i]);
      writeFloat(0+(16*i),tongDiennang[i]);
      if(day()==dayWrite){
        Serial.println("Ghi số tháng mới!");
        writeFloat(12+(16*i),csThangroi[i]);
        writeFloat(8+(16*i),tongDiennang[i]);
      }
    }
    EEPROM.commit();
    readChiSo();
  }
}
void readChiSo(){
  for(int i=0;i<NUM_PZEMS;i++){
    csHomqua[i] = readFloat(0+(16*i));
    csHomtruoc[i] = readFloat(4+(16*i));
    csThangroi[i] = readFloat(8+(16*i));
    csThangtruoc[i] = readFloat(12+(16*i));
    Serial.println();
    Serial.print("Chỉ số hôm qua    "+String(i)+": ");Serial.println(csHomqua[i]);
    Serial.print("Chỉ số hôm trước  "+String(i)+": ");Serial.println(csHomtruoc[i]);
    Serial.print("Chỉ số tháng rồi  "+String(i)+": ");Serial.println(csThangroi[i]);
    Serial.print("Chỉ số tháng trước"+String(i)+": ");Serial.println(csThangtruoc[i]);
  }
}
float readFloat(unsigned int addr){
  union{
    byte b[4];
    float f;
  }data;
  for(int i=0; i<4; i++){
    data.b[i]=EEPROM.read(addr+i);
  }
  return data.f;
}
void writeFloat(unsigned int addr, float x){
  union{
    byte b[4];
    float f;
  }data;
  data.f=x;
  for(int i=0; i<4;i++){
    EEPROM.write(addr+i,data.b[i]);
  }
}
BLYNK_CONNECTED() {
  rtc.begin();  //Khởi chạy hệ thống thời gian thực
  Blynk.syncVirtual(DayWrite); //Đồng bộ ngày chốt số điện cho tháng
  LedReset.off();
}
BLYNK_WRITE(DayWrite){
  dayWrite = param.asInt(); //Lấy ngày chốt số điện từ blynk về
  Serial.println("Ngày chốt số: "+String(dayWrite));
}
BLYNK_WRITE(Reset){
  clearState  = param.asInt();
  pushTime=millis();
}