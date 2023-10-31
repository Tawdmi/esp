#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
//#include <WebSocketsServer.h>
#include <ESP8266mDNS.h>
#include <EEPROM.h>
#include <ESP8266HTTPUpdateServer.h>
#include <BlynkSimpleEsp8266.h>
#include <TimeLib.h>
#include <WidgetRTC.h>
#include <PZEM004Tv30.h>
#include "HtmlHomePage.h"
#include "HtmlSetupPage.h"
#include "HtmlLoginPage.h"
#define buttonReset 16        //Nút nhấn reset về mặc định D0
#define timeReconnectWL 300000L        //Thời gian reconnect wifi là 5 phút
#define timeReconnectBL 120000L
#define timeUpdateBL 1000L
#define ledSignal 2           //Led báo trạng thái hoạt động D4
#define output1 D4
#define output2 D8
//--------pzem--------------------
#define Rx1 D1     //--->Tx1 pzem
#define Tx1 D2     //--->Rx1 pzem
#define Rx2 D3     //--->Tx2 pzem
#define Tx2 D5     //--->Rx2 pzem
#define Rx3 D6     //--->Tx3 pzem
#define Tx3 D7     //--->Rx3 pzem
PZEM004Tv30 pzem1(Rx1, Tx1);
PZEM004Tv30 pzem2(Rx2, Tx2);
PZEM004Tv30 pzem3(Rx3, Tx3);
float voltage1=0;
float current1=0;
float power1=0;
float energy1=0;
float frequency1=0;
float pf1=0;
float csHomqua1, csHomtruoc1, csThangroi1, csThangtruoc1;
float voltage2=0;
float current2=0;
float power2=0;
float energy2=0;
float frequency2=0;
float pf2=0;
float csHomqua2, csHomtruoc2, csThangroi2, csThangtruoc2;
float voltage3=0;
float current3=0;
float power3=0;
float energy3=0;
float frequency3=0;
float pf3=0;
float csHomqua3, csHomtruoc3, csThangroi3, csThangtruoc3;
int ngayChotSo=12;
int gioChotSo=23, phutChotSo=59;
boolean resetE = false;
boolean savedata = false;
WidgetRTC rtc;
unsigned long times_pzem = millis();
int buttonResetE=0;
int countResetE=0;
WidgetLED ledReset(V12);
//--------------------------------------
ESP8266WebServer webServer(80);
//WebSocketsServer webSocket = WebSocketsServer(81);
String ssid_ap, pass_ap, user_login, pass_login, active_code;
String ssid_sta, pass_sta, auth_token;
String firmware_info = "3Pzem004Tv1.0";
//--------------OTA-----------------//
ESP8266HTTPUpdateServer httpUpdater;
const char* updatePath = "/update";
//const char* updateUsername = "admin";
//const char* updatePassword = "admin";
//-----------------------------------//
unsigned long times = millis();
unsigned long times1 = millis();
unsigned long times2 = millis();
unsigned long times3 = millis();
unsigned long timesUpdateData=millis();
unsigned long timesSaveData=millis();
unsigned long uptimes = millis();
unsigned long timesUpdateBlynk = millis();
unsigned long timesResetEnergy=millis();
String uptimesString;
boolean reConnectState=0;
boolean reStoreConfigState=0;
//---------------BLYNK---------------//
boolean blynkConnectState=0;
//BlynkTimer timer;
//int timerID1, timerID2, timerID3, timerID4;
WidgetLED ledConnect(V0);
boolean ledconnect=0;
boolean connectBlynkState=0;
//-------------ACTIVE CODE----------//
unsigned int chip_id=0;
boolean activestate=0;
int mua1 = 1678;
int mua2 = 1734;
int mua3 = 2014;
int mua4 = 2536;
int mua5 = 2834;
int mua6 = 2927;
int ban = 1938;


void setup() {
  Serial.begin(9600);
  Serial.println();
  EEPROM.begin(512);
  delay(10);
  pinMode(ledSignal,OUTPUT);
  digitalWrite(ledSignal,LOW);
  pinMode(buttonReset,INPUT_PULLDOWN_16);
  pinMode(output1,OUTPUT);
  pinMode(output2,OUTPUT);
  digitalWrite(output1,HIGH);
  digitalWrite(output2,HIGH);
  //======================================================//
  if(reStoreConfig()){
    reStoreConfigState=1;
    Serial.println();Serial.println("**==============================================**");
    Serial.println("**           SETUP DEVICE CONFIGURATION         **");
    Serial.println("**==============================================**");
    if(checkConnectWifi()){ //Nếu kết nối wifi thành công thì kiểm tra kết nối đến server blynk
      if(!checkConnectBlynk()){
        reConnectState=1;
        Serial.println("ReConnect to blynk server is started!");
        times1=millis();
      }
    }else{
      startAccesspointMode(); //Không kết nối được wifi thì phát ra 1 mạng wifi cho người dùng
      reConnectState=1;
      Serial.println("ReConnect to wifi is started!");
      times1=millis();
    }
  }else{
    startAccesspointMode();
  }
  
  setSyncInterval(60*60);
  readChiSo();
 
  startmDNS();
  startWebServer();
  httpUpdater.setup(&webServer, updatePath, user_login, pass_login);
  //webSocket.begin();
  //webSocket.onEvent(webSocketEvent);

  activestate = 1;
  Serial.print("Active state: "); Serial.println(activestate);
}

void loop() {
  webServer.handleClient();
  MDNS.update();
 // webSocket.loop();
  uptimes = millis();
  String uptimeMn, uptimeHr, uptimeDy;
  uptimeMn = int((uptimes/(1000*60))%60);
  uptimeHr = int((uptimes/(1000*60*60))%24);
  uptimeDy = int((uptimes/(1000*60*60*24))%365);
  uptimesString = uptimeDy + "d : " + uptimeHr +"h : " +uptimeMn+"m";
  //-----------Kiểm tra kết nối wifi và blynk------------------------//
  if(reStoreConfigState==1){
    if(WiFi.isConnected()==true){
      if(Blynk.connected()){
        Blynk.run();
        if(millis()-times3>timeUpdateBL){
          updateBlynk();
          times3 = millis();
        }
        if(buttonResetE==1){
          if(millis()-timesResetEnergy>3000){
            ledReset.on();
            pzem1.resetEnergy();
            delay(100);
            pzem2.resetEnergy();
            delay(100);
            pzem3.resetEnergy();
            delay(100);
            for (int i = 288; i < 336; ++i) {
            EEPROM.write(i, 0);           
              delay(10);
              digitalWrite(ledSignal,!digitalRead(ledSignal));
            }
            EEPROM.commit();
            Serial.println("Đã clear pzem!");
            readChiSo();
          }
        }else{
          timesResetEnergy=millis();
          ledReset.off();
        }
      }else{
        if(reConnectState==0){
          reConnectState=1;
          Serial.println();
          Serial.println("ReConnect to blynk server is started!");
          reConnect();
          times1=millis();
        }else{
          if(millis()-times1>timeReconnectBL){
            times1=millis();
            reConnect();
            times1 = millis();
          }
        }
      }
    }else{
      if(reConnectState==0){
        reConnectState=1;
        Serial.println();
        Serial.println("ReConnect to wifi is started!");
        reConnect();
        times1=millis();
      }else{
        if(millis()-times1>timeReconnectWL){
          reConnect();
          times1=millis();
        }
      }
    }
  }
  //-----------Sự kiện resetDefault-------------------//
  if(digitalRead(buttonReset)==HIGH){
    if(millis()-times>2000){
      for(int i=0;i<30;i++){
        delay(100);
        digitalWrite(ledSignal,!digitalRead(ledSignal));
      }
      resetDefault();
      ESP.restart();
    }
  }else{
    times = millis();
  }
  //------------Đọc pzem-----------------------------//
  if(millis()-times_pzem>1000){
    times_pzem = millis();
    readPzem1();
    delay(10);
    readPzem2();
    delay(10);
    readPzem3();
    delay(10);
    
    Blynk.virtualWrite(V1, voltage1);
    Blynk.virtualWrite(V2, current1);
    Blynk.virtualWrite(V3, power1);
    Blynk.virtualWrite(V4, frequency1);
    Blynk.virtualWrite(V5, pf1);
    Blynk.virtualWrite(V6, energy1);
    
    Blynk.virtualWrite(V21, voltage2);
    Blynk.virtualWrite(V22, current2);
    Blynk.virtualWrite(V23, power2);
    Blynk.virtualWrite(V24, frequency2);
    Blynk.virtualWrite(V25, pf2);
    Blynk.virtualWrite(V26, energy2);
    
    Blynk.virtualWrite(V31, voltage3);
    Blynk.virtualWrite(V32, current3);
    Blynk.virtualWrite(V33, power3);
    Blynk.virtualWrite(V34, frequency3);
    Blynk.virtualWrite(V35, pf3);
    Blynk.virtualWrite(V36, energy3);
    float homnay1 = energy1-csHomqua1;
    float homqua1 = csHomqua1-csHomtruoc1;
    float thangnay1 = energy1-csThangroi1;
    float thangtruoc1 = csThangroi1-csThangtruoc1;
    
    float homnay2 = energy2-csHomqua2;
    float homqua2 = csHomqua2-csHomtruoc2;
    float thangnay2 = energy2-csThangroi2;
    float thangtruoc2 = csThangroi2-csThangtruoc2;
    
    float homnay3 = energy3-csHomqua3;
    float homqua3 = csHomqua3-csHomtruoc3;
    float thangnay3 = energy3-csThangroi3;
    float thangtruoc3 = csThangroi3-csThangtruoc3;

    Blynk.virtualWrite(V41, homnay1);
    Blynk.virtualWrite(V42, homqua1);
    Blynk.virtualWrite(V43, thangnay1);
    Blynk.virtualWrite(V44, thangtruoc1);
    Blynk.virtualWrite(V45, tinhtien(thangnay1));
    Blynk.virtualWrite(V46, tinhtien(thangtruoc1));

    Blynk.virtualWrite(V51, homnay2);
    Blynk.virtualWrite(V52, homqua2);
    Blynk.virtualWrite(V53, thangnay2);
    Blynk.virtualWrite(V54, thangtruoc2);
    Blynk.virtualWrite(V55, tinhtien(thangnay2));
    Blynk.virtualWrite(V56, tinhtien(thangtruoc2));

    Blynk.virtualWrite(V61, homnay3);
    Blynk.virtualWrite(V62, homqua3);
    Blynk.virtualWrite(V63, thangnay3);
    Blynk.virtualWrite(V64, thangtruoc3);
    Blynk.virtualWrite(V65, tinhtien(thangnay3));
    Blynk.virtualWrite(V66, tinhtien(thangtruoc3));
  }
  if(millis()-timesSaveData>60000){
    timesSaveData=millis();
    writeChiSo();
  }
}
int tinhtien(float sodien){
  int thanhtien;
  if(sodien>400){
    thanhtien = (sodien-400)*mua6 + 100*mua5 + 100*mua4 + 100*mua3 + 50*mua2 + 50*mua1;
  }else if(sodien>300){
    thanhtien = (sodien-300)*mua5 + 100*mua4 + 100*mua3 + 50*mua2 + 50*mua1;
  }else if(sodien>200){
    thanhtien = (sodien-200)*mua4 + 100*mua3 + 50*mua2 + 50*mua1;
  }else if(sodien>100){
    thanhtien = (sodien-100)*mua3 + 50*mua2 + 50*mua1;
  }else if(sodien>50){
    thanhtien = (sodien-50)*mua2  + 50*mua1;
  }else{
    thanhtien = sodien*mua1;
  }
  return thanhtien;
}
//================Chương trình con=======================//
void reConnect(){
  if(WiFi.isConnected()==true){
    if(checkConnectBlynk()){
      times1=millis();
      reConnectState=0;
    }
  }else{
    if(!checkConnectWifi()){
      startAccesspointMode();
    }else{
      if(checkConnectBlynk()){
        times1=millis();
        reConnectState=0;
      }
    }
  }
}
boolean checkConnectBlynk(){
  Blynk.config(auth_token.c_str(),"blynk-cloud.com", 80);
  //Blynk.config(auth_token.c_str(),"192.168.0.123", 8080);
  //Serial.println(); Serial.print("Connecting to Blynk server");
  Blynk.connect();
  int n=0;
  while(n<15){
    if(Blynk.connected()){
      return true;
    }
    delay(200);
    //Serial.print(".");
    n++;
  }
  //Serial.println();Serial.println("The connection process is timed out!");
  return false;
}
void resetDefault(){
  for(int i=0; i<512;++i){
    EEPROM.write(i,0);
    delay(10);
  }
  ssid_ap = "HOME-Node00";
  pass_ap = "12345678";
  user_login = "admin";
  pass_login = "admin";
  for(int i=0;i<ssid_ap.length();++i){
    EEPROM.write(i,ssid_ap[i]);
  }
  for(int i=0;i<pass_ap.length();++i){
    EEPROM.write(32+i,pass_ap[i]);
  }
  for(int i=0;i<user_login.length();++i){
    EEPROM.write(96+i,user_login[i]);
  }
  for(int i=0;i<pass_login.length();++i){
    EEPROM.write(128+i,pass_login[i]);
  }
  EEPROM.commit();
  Serial.println("Done resetting the device to its default state!");
}
boolean reStoreConfig(){
  Serial.println(); 
  Serial.println("**==============================================**");
  Serial.println("**               GIAM SAT CONG TO               **");
  Serial.println("**==============================================**");
  Serial.println("Reading data from EEPROM memory...!");
  for(int i=0; i<32;++i){
    ssid_ap += char(EEPROM.read(i));
  }
  for(int i=32; i<96;++i){
    pass_ap += char(EEPROM.read(i));
  }
  for(int i=96; i<128;++i){
    user_login += char(EEPROM.read(i));
  }
  for(int i=128; i<160;++i){
    pass_login += char(EEPROM.read(i));
  }
  for(int i=480;i<512;++i){
    active_code +=char(EEPROM.read(i));
  }
  ssid_ap = ssid_ap.c_str();
  pass_ap = pass_ap.c_str();
  user_login = user_login.c_str();
  pass_login = pass_login.c_str();
  active_code = active_code.c_str();
  Serial.println("**------------------ACTIVE CODE-----------------**");
  Serial.print("ACTIVE CODE: "); Serial.println(active_code);
  Serial.println("**----------------ACCESSPOINT MODE--------------**");
  Serial.print("SSID ACCESSPOINT: "); Serial.println(ssid_ap);
  Serial.print("PASSWORD: "); Serial.println(pass_ap);
  Serial.println("**---------------ACCESS WEB SERVER--------------**");
  Serial.print("USER LOGIN: "); Serial.println(user_login);
  Serial.print("PASSWORD: "); Serial.println(pass_login);
  Serial.println("**----------------UPDATE OTA INFO---------------**");
  Serial.print("URL WEB SERVER: "); Serial.print(WiFi.softAPIP()); Serial.println(updatePath);
  Serial.print("USER LOGIN: "); Serial.println(user_login);
  Serial.print("PASSWORD: "); Serial.println(pass_login);
  
  if(EEPROM.read(160)!=0){
    for(int i=160; i<192;++i){
      ssid_sta += char(EEPROM.read(i));
    }
    ssid_sta = ssid_sta.c_str();
    Serial.println("**-----------------STATION MODE-----------------**");
    Serial.print("SSID STATION: "); Serial.println(ssid_sta);
    if(EEPROM.read(192)!=0){
      for(int i=192; i<256;++i){
        pass_sta += char(EEPROM.read(i));
      }
      pass_sta = pass_sta.c_str();
      Serial.print("PASSWORD: "); Serial.println(pass_sta);
    }
    if(EEPROM.read(256)!=0){
      for(int i=256; i<288;++i){
        auth_token += char(EEPROM.read(i));
      }
      auth_token = auth_token.c_str();
      Serial.print("AUTH TOKEN: "); Serial.println(auth_token);
    }
    return true;
  }else{
    Serial.println("**-----------------STATION MODE-----------------**");
    Serial.println("Station mode configuration has not been set up!");
    return false;
  }
}
boolean checkConnectWifi(){
  //Serial.print("Connecting to a wifi network");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid_sta,pass_sta);
  int n=0;
  while(n<20){
    if(WiFi.status()==WL_CONNECTED){
      //Serial.println();Serial.print("CONNECTED TO "); Serial.println(ssid_sta);
      //Serial.print("IP CONNECTED: ");Serial.println(WiFi.localIP());
      return true;
    }
    n++;
    delay(500);
    //Serial.print(".");
  }
  //Serial.println();Serial.println("The wifi connection process is timed out!");
  return false;
}
void startAccesspointMode(){
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid_ap,pass_ap, 1, false,4);
  Serial.println("**----------ACCESSPOINT MODE ACTIVATED----------**");
  Serial.println("Please connect to the device's wifi to configure!");
  Serial.print("SSID ACCESSPOINT: ");Serial.println(ssid_ap);
  Serial.print("PASSWORD: "); Serial.println(pass_ap);
  Serial.print("URL WEB SERVER: "); Serial.println(WiFi.softAPIP());
  Serial.print("USER LOGIN: "); Serial.println(user_login);
  Serial.print("PASSWORD: "); Serial.println(pass_login);
}
void startmDNS(){
  if (!MDNS.begin(firmware_info)) {             // Start the mDNS responder for esmart.local
    Serial.println("Error setting up MDNS responder!");
  }
  Serial.println();Serial.println("The mDNS responder started!");
  String m = "Hosting name is http://"+ firmware_info + ".local";
  Serial.println(m);
  MDNS.addService("http","tcp",80);
}
void startWebServer(){
  webServer.on("/",[]{
    String s = FPSTR(LoginPage);
    webServer.send(200,"text/html",s);
  });
  webServer.on("/home",[]{
    String s = FPSTR(HomePage);
    webServer.send(200,"text/html",s);
  });
  webServer.on("/login",[]{
    String s = FPSTR(LoginPage);
    webServer.send(200,"text/html",s);
  });
  //---------------------------------------------------
  webServer.on("/setup",[]{
    String s = FPSTR(SetupPage);
    webServer.send(200,"text/html",s);
  });
  webServer.onNotFound([]() {
    String s = FPSTR(HomePage);
    webServer.send(200,"text/html",s);
  });
  webServer.on("/getSystem",getSystem);
  webServer.on("/saveLogin",saveLogin);
  webServer.on("/getConfig",getConfig);
  webServer.on("/saveConfig",saveConfig);
  webServer.on("/getBanggia",getBanggia);
  webServer.on("/saveBanggia",saveBanggia);
  webServer.on("/reStart",reStart);
  
  webServer.begin();
}
void getSystem(){
  String s = "{\"user_login\": \""+ user_login +"\"," +
              "\"pass_login\": \""+ pass_login +"\"," +
              "\"active_state\": \""+ activestate +"\"," +
              "\"firmware_info\": \""+ firmware_info + "\"}";
  webServer.send(200,"application/json",s);
  //Serial.println(s);
}
void saveLogin(){
  user_login = webServer.arg("user_login");
  pass_login = webServer.arg("pass_login");
  for(int i=96; i<160;++i){
    EEPROM.write(i,0);
    delay(10);
  }
  for(int i=0;i<user_login.length();++i){
    EEPROM.write(96+i,user_login[i]);
  }
  for(int i=0;i<pass_login.length();++i){
    EEPROM.write(128+i,pass_login[i]);
  }
  EEPROM.commit();
  String s = "Thông tin đăng nhập hệ thống đã được lưu thành công!";
  webServer.send(200,"text/html",s);
}
void getConfig(){
  int n = WiFi.scanNetworks();
  delay(100);
  String ssidList = "[\"";                //"[wifi1,wifi2,wifi3]"
  for(int i=0;i<n;++i){
    ssidList+= WiFi.SSID(i)+ "\"";        //{"keyword1":"value","keyword2":"value","keyword3":["wifi1","wifi2",...]}
    if(i<(n-1)){
      ssidList += ",\"";
    }
  }
  ssidList += "]";
  String s = "{\"ssid_ap\": \""+ ssid_ap +"\"," +
              "\"pass_ap\": "+ pass_ap + "," +
              "\"ssid_sta\": \""+ ssid_sta + "\"," +
              "\"pass_sta\": \""+ pass_sta + "\"," +
              "\"auth_token\": \""+ auth_token + "\"," +
              "\"ssidList\": "+ ssidList + "}";
  webServer.send(200,"application/json",s);
  //Serial.println(s);
}
void saveConfig(){
  ssid_ap = webServer.arg("ssid_ap");
  pass_ap = webServer.arg("pass_ap");
  ssid_sta = webServer.arg("ssid_sta");
  pass_sta = webServer.arg("pass_sta");
  auth_token = webServer.arg("auth_token");
  for(int i=0; i<96;++i){
    EEPROM.write(i,0);
    delay(10);
  }
  for(int i=160; i<288;++i){
    EEPROM.write(i,0);
    delay(10);
  }
  for(int i=0;i<ssid_ap.length();++i){
    EEPROM.write(i,ssid_ap[i]);
  }
  for(int i=0;i<pass_ap.length();++i){
    EEPROM.write(32+i,pass_ap[i]);
  }
  for(int i=0;i<ssid_sta.length();++i){
    EEPROM.write(160+i,ssid_sta[i]);
  }
  for(int i=0;i<pass_sta.length();++i){
    EEPROM.write(192+i,pass_sta[i]);
  }
  for(int i=0;i<auth_token.length();++i){
    EEPROM.write(256+i,auth_token[i]);
  }
  EEPROM.commit();
  String s = "Cấu hình đã được lưu thành công. Vui lòng khởi động lại thiết bị!";
  webServer.send(200,"text/html",s);
}
void getBanggia(){
  String s = "{\"mua1\": \""+ String(mua1) +"\"," +
              "\"mua2\": \""+ mua2 + "\"," +
              "\"mua3\": \""+ mua3 + "\"," +
              "\"mua4\": \""+ mua4 + "\"," +
              "\"mua5\": \""+ mua5 + "\"," +
              "\"mua6\": \""+ mua6 + "\"," +
              "\"ban\": \""+ ban + "\"}";
  webServer.send(200,"application/json",s);
  //Serial.println(s);
}
void saveBanggia(){
  mua1 = webServer.arg("muc1").toInt();
  mua2 = webServer.arg("muc2").toInt();
  mua3 = webServer.arg("muc3").toInt();
  mua4 = webServer.arg("muc4").toInt();
  mua5 = webServer.arg("muc5").toInt();
  mua6 = webServer.arg("muc6").toInt();
  ban = webServer.arg("ban").toInt();
  writeInt(336,mua1);
  writeInt(338,mua2);
  writeInt(340,mua3);
  writeInt(342,mua4);
  writeInt(344,mua5);
  writeInt(346,mua6);
  writeInt(348,ban);
  EEPROM.commit();
  String s = "Đã lưu bảng giá thành công!";
  webServer.send(200,"text/html",s);
}
void reStart(){
  ESP.restart();
}
//---------------Nhận socket từ client gửi lên------//
//void webSocketEvent(uint8_t num, WStype_t type,uint8_t * payload,size_t length){
//  String payloadString = (const char *)payload;
 // Serial.print("payloadString= ");
 // Serial.println(payloadString);
//  if(payloadString == "output1_on"){            
//    digitalWrite(output1,LOW);
//    Blynk.virtualWrite(V1, 0);
//  }else if(payloadString == "output1_off"){
//    digitalWrite(output1,HIGH);
//    Blynk.virtualWrite(V1, 1);
//  }
//}
//void updateWebserver(){
//  ledconnect = !ledconnect;
//  String JSONtxt = "{\"ledconnect\": \""+ String(ledconnect) +"\"," +
//                    "\"uptimes\": \""+ uptimesString + "\"," +
//                    "\"ipstation\": \""+ WiFi.localIP().toString() +"\"}";
//  webSocket.broadcastTXT(JSONtxt);
  //Serial.println(JSONtxt);
//}
//-------------pzem-----------------------
//==============Chương trình con======================//
void readPzem1(){
    float temp = pzem1.voltage();
    if( !isnan(temp) ){
        voltage1 = temp;
    }
    temp = pzem1.current();
    if( !isnan(temp) ){
        current1 = temp;
    }
    temp = pzem1.power();
    if( !isnan(temp) ){
        power1 = temp;   
    }
    temp = pzem1.energy();
    if( !isnan(temp) ){
        energy1 = temp; 
    }
    temp = pzem1.frequency();
    if( !isnan(temp) ){
        frequency1 = temp; 
    }
    temp = pzem1.pf();
    if( !isnan(temp) ){
        pf1 = temp;
    }
//    Serial.println();
//    Serial.print("Voltage1: "); Serial.print(voltage1); Serial.println("V");
//    Serial.print("Current1: "); Serial.print(current1); Serial.println("A");
//    Serial.print("Power1: "); Serial.print(power1); Serial.println("W");
//    Serial.print("Energy1: "); Serial.print(energy1,3); Serial.println("kWh");
//    Serial.print("Frequency1: "); Serial.print(frequency1, 1); Serial.println("Hz");
//    Serial.print("PF1: "); Serial.println(pf1);
}
void readPzem2(){
    float temp = pzem2.voltage();
    if( !isnan(temp) ){
        voltage2 = temp;
    }
    temp = pzem2.current();
    if( !isnan(temp) ){
        current2 = temp;
    }
    temp = pzem2.power();
    if( !isnan(temp) ){
        power2 = temp;   
    }
    temp = pzem2.energy();
    if( !isnan(temp) ){
        energy2 = temp; 
    }
    temp = pzem2.frequency();
    if( !isnan(temp) ){
        frequency2 = temp; 
    }
    temp = pzem2.pf();
    if( !isnan(temp) ){
        pf2 = temp;
    }
//    Serial.println();
//    Serial.print("Voltage2: "); Serial.print(voltage2); Serial.println("V");
//    Serial.print("Current2: "); Serial.print(current2); Serial.println("A");
//    Serial.print("Power2: "); Serial.print(power2); Serial.println("W");
//    Serial.print("Energy2: "); Serial.print(energy2,3); Serial.println("kWh");
//    Serial.print("Frequency2: "); Serial.print(frequency2, 1); Serial.println("Hz");
//    Serial.print("PF2: "); Serial.println(pf2);
}
void readPzem3(){
    float temp = pzem3.voltage();
    if( !isnan(temp) ){
        voltage3 = temp;
    }
    temp = pzem3.current();
    if( !isnan(temp) ){
        current3 = temp;
    }
    temp = pzem3.power();
    if( !isnan(temp) ){
        power3 = temp;   
    }
    temp = pzem3.energy();
    if( !isnan(temp) ){
        energy3 = temp; 
    }
    temp = pzem3.frequency();
    if( !isnan(temp) ){
        frequency3 = temp; 
    }
    temp = pzem3.pf();
    if( !isnan(temp) ){
        pf3 = temp;
    }
//    Serial.println();
//    Serial.print("Voltage3: "); Serial.print(voltage3); Serial.println("V");
//    Serial.print("Current3: "); Serial.print(current3); Serial.println("A");
//    Serial.print("Power3: "); Serial.print(power3); Serial.println("W");
//    Serial.print("Energy3: "); Serial.print(energy3,3); Serial.println("kWh");
//    Serial.print("Frequency3: "); Serial.print(frequency3, 1); Serial.println("Hz");
//    Serial.print("PF3: "); Serial.println(pf3);
}
//-------------BLYNK----------------------
BLYNK_CONNECTED() {
  rtc.begin();
  Blynk.syncVirtual(V10,V7,V8);
}
void updateBlynk(){
  if (ledConnect.getValue()) {
    ledConnect.off();
  } else {
    ledConnect.on();
  }
  Blynk.virtualWrite(V13, uptimesString);
  Blynk.virtualWrite(V14, WiFi.localIP().toString());
}
BLYNK_WRITE(V7){
  digitalWrite(output1,param.asInt());
  if(digitalRead(output1)==LOW){
    Serial.println("Đã bật output 1!");
    Blynk.virtualWrite(V8, HIGH);
    digitalWrite(output2,HIGH);
  }
}
BLYNK_WRITE(V8){
  digitalWrite(output2,param.asInt());
  if(digitalRead(output2)==LOW){
    Serial.println("Đã bật output 2!");
    Blynk.virtualWrite(V7, HIGH);
    digitalWrite(output1,HIGH);
  }
}
BLYNK_WRITE(V10){
  ngayChotSo = param.asInt();
}
BLYNK_WRITE(V11){
  buttonResetE = param.asInt();
}
//-------------Ghi dữ liệu kiểu float vào bộ nhớ EEPROM----------------------//
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
void writeInt(int address, int number){
  byte byte1 = number>>8;
  byte byte2 = number&0xFF;
  EEPROM.write(address,byte1);
  EEPROM.write(address+1,byte2);
}
int readInt(int address){
  byte byte1 = EEPROM.read(address);
  byte byte2 = EEPROM.read(address+1);
  return ((byte1<<8)+byte2);
}
void readChiSo(){
  csHomqua1 = readFloat(288);
  csHomtruoc1 = readFloat(292);
  csThangroi1 = readFloat(296);
  csThangtruoc1 = readFloat(300);
  
  csHomqua2 = readFloat(304);
  csHomtruoc2 = readFloat(308);
  csThangroi2 = readFloat(312);
  csThangtruoc2 = readFloat(316);
  
  csHomqua3 = readFloat(320);
  csHomtruoc3 = readFloat(324);
  csThangroi3 = readFloat(328);
  csThangtruoc3 = readFloat(332);
//  Serial.println();
//  Serial.print("Chỉ số hôm qua: ");Serial.println(csHomqua1);
//  Serial.print("Chỉ số hôm trước: ");Serial.println(csHomtruoc1);
//  Serial.print("Chỉ số tháng rồi: ");Serial.println(csThangroi1);
//  Serial.print("Chỉ số tháng trước: ");Serial.println(csThangtruoc1);
  mua1 = readInt(336);
  mua2 = readInt(338);
  mua3 = readInt(340);
  mua4 = readInt(342);
  mua5 = readInt(344);
  mua6 = readInt(346);
  ban = readInt(348);
}
void writeChiSo(){
  if(hour()==gioChotSo && minute() == phutChotSo){
    //Serial.println("Ghi số ngày mới!");
    writeFloat(292,csHomqua1);
    writeFloat(288,energy1);
    //2
    writeFloat(308,csHomqua2);
    writeFloat(304,energy2);
    //3
    writeFloat(324,csHomqua3);
    writeFloat(320,energy3);
      
    if(day()==ngayChotSo){
      //Serial.println("Ghi số tháng mới!");
      writeFloat(300,csThangroi1);
      writeFloat(296,energy1);
      //2
      writeFloat(216,csThangroi2);
      writeFloat(312,energy2);
      //3
      writeFloat(332,csThangroi3);
      writeFloat(328,energy3);
    }
    EEPROM.commit();
    readChiSo();
  }
}
