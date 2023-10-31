int x =0;
int hr1,hr12;

#include<dht.h>
#define dht_apin 5
dht DHT;
int temp,hum;
#include <Wire.h>
#include "RTClib.h"
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------The SPI, DMD, TimerOne and Font libraries are used.
#include <SPI.h>       
#include <DMD.h>    
#include <TimerOne.h>  
#include "SystemFont5x7.h"
#include "Font_6x14.h" //-> This font only contains numbers from 0-9
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------Configuration P10
#define DISPLAYS_ACROSS 1 //-> Number of P10 panels used, side to side.
#define DISPLAYS_DOWN 1
DMD dmd(DISPLAYS_ACROSS, DISPLAYS_DOWN);
//--------------------------------------------------------------------------------------

RTC_DS1307 rtc; //-> RTC Declaration

//--------------------------------------------------------------------------------------Variables for time and date
int _day, _month, _year, _hour24, _hour12, _minute, _second, _dtw;
int hr24;
String st;
char nameoftheday[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
char month_name[12][12] = {"January","February", "March", "April", "May", "June", "Jult", "August", "September", "October", "November", "December"};
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------Variable for Millis
const long interval = 1000; //-> Retrieve time and date data every 1 second
unsigned long previousMillis = 0;

const long interval_for_date = 75; //-> For scroll speed
unsigned long previousMillis_for_date = 0;
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------Variable to display hour, minute and second
char hr_24 [3];
String str_hr_24;
char mn [3];
String str_mn;
char sc [3];
String str_sc;
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------ScanDMD()
void ScanDMD() { 
  dmd.scanDisplayBySPI();
  Timer1.pwm(PIN_DMD_nOE, 30);  
}
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------setup
void setup() {
  Serial.begin(115200);
  Serial.println("Arduino RTC DS1307");
  delay(1000);
  
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled (Set the time and date based on your computer time and date)
// rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); //-> Set the time and date based on your computer time and date. If that doesn't work, use this line of code outside of "if (! rtc.isrunning())"
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
  // following line sets the RTC to the date & time this sketch was compiled (Set the time and date based on your computer time and date)
  //rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); //-> Set the time and date based on your computer time and date. Use this line of code if it doesn't work in "if (! rtc.isrunning())"
  //rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  // If the time and date are successfully set, then deactivate the code line (make the code a comment), then re-upload the code.
  // if not done then the time and date will return to the beginning when it was set when arduino is reset or restarted.
   Serial.begin(9600);
   DHT.read11(dht_apin);  
   delay(2000); 
  Timer1.initialize(2000);        
  Timer1.attachInterrupt(ScanDMD);   
  dmd.clearScreen(true);  
  
}
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------loop
void loop() {
     DHT.read11(dht_apin);  
   temp = DHT.temperature;
   hum = DHT.humidity;
  //_____________________________________________________millis() to display time
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis; //-> save the last time

    GetDateTime(); //-> Retrieve time and date data from DS1307

    //=====================================================Showing hour in P10
    dmd.selectFont(Font_6x14);
    
    str_hr_24=String(_hour24);
    str_hr_24.toCharArray(hr_24,3);

    if (_hour24<10) {
      dmd.drawString(1, 0, "0", 1, GRAPHICS_NORMAL);
      dmd.drawString(8, 0, hr_24, 1, GRAPHICS_NORMAL);
    }
    else {
      dmd.drawString(1, 0, hr_24, 2, GRAPHICS_NORMAL);
    }
    //=====================================================

    //=====================================================Showing ":" in P10
    dmd.drawFilledBox(16,3,17,4, GRAPHICS_NORMAL);
    dmd.drawFilledBox(16,11,17,12, GRAPHICS_NORMAL);
    //=====================================================

    //=====================================================Showing minute and second in P10
    dmd.selectFont(SystemFont5x7);
    
    str_mn=String(_minute);
    str_mn.toCharArray(mn,3);

    if (_minute<10) {
      dmd.drawString(20, 0, "0", 1, GRAPHICS_NORMAL);
      dmd.drawString(26, 0, mn, 1, GRAPHICS_NORMAL);
    }
    else {
      dmd.drawString(20, 0, mn, 2, GRAPHICS_NORMAL);
    }

    str_sc=String(_second);
    str_sc.toCharArray(sc,3);

    if (_second<10) {
      dmd.drawString(20, 9, "0", 1, GRAPHICS_NORMAL);
dmd.drawString(26, 9, sc, 1, GRAPHICS_NORMAL);
    }
    else {
      dmd.drawString(20, 9, sc, 2, GRAPHICS_NORMAL);
    }
    //=====================================================

    //=====================================================Call the scrolling_date() subroutine to display the date.
    if (_second==11) { //-> Display the date when seconds equal to 11
      scrolling_date();
    }
    //=====================================================
  }
  //_____________________________________________________
}
//--------------------------------------------------------------------------------------

//------------------------------------------------------------------------Subroutine to retrieve or update the time and date from DS1307
void GetDateTime() {
  DateTime now = rtc.now();
  _day=now.day();
  _month=now.month();
  _year=now.year();
  _hour24=now.hour();
  _minute=now.minute();
  _second=now.second();
  _dtw=now.dayOfTheWeek();

  hr24=_hour24;
  if (hr24>12) {
    _hour12=hr24-12;
  }
  else if (hr24==0) {
    _hour12=12;
  }
  else {
    _hour12=hr24;
  }

  if (hr24<12) {
    st="AM";
  }
  else {
    st="PM"; 
  }  
     DHT.read11(dht_apin);  
//  temp = DHT.temperature;
 // hum = DHT.humidity;
  Serial.println(temp);
  Serial.println(hum);
  //delay(2000);
  
}
//------------------------------------------------------------------------

//------------------------------------------------------------------------Subroutine to display days, months and years
void scrolling_date() {
    char t [3];
  String str2;
  str2=String(temp);
  str2.toCharArray(t,3);
  char h [3];
  String str3;
  str3=String(hum);
  str3.toCharArray(h,3);
 
 
   
 
     dmd.clearScreen( true );
     dmd.selectFont(System5x7);
     for (byte x=0;x<DISPLAYS_ACROSS;x++) {
     for (byte y=0;y<DISPLAYS_DOWN;y++)   {
     dmd.drawString(  0+(32*x),  0+(16*y), "T:", 2, GRAPHICS_NORMAL );
     dmd.drawString(  0+(32*x),  9+(16*y), "H:", 2, GRAPHICS_NORMAL );
     }
     }
    
     dmd.selectFont(System5x7);
     for (byte x=0;x<DISPLAYS_ACROSS;x++) {
     for (byte y=0;y<DISPLAYS_DOWN;y++)   {
     dmd.drawString(  11+(32*x),  0+(16*y), t, 3, GRAPHICS_NORMAL );
     dmd.drawString(  11+(32*x),  9+(16*y), h, 3, GRAPHICS_NORMAL );
     }
     }

     dmd.drawCircle( 24,  1,  1, GRAPHICS_NORMAL );
     
     dmd.selectFont(System5x7);
     for (byte x=0;x<DISPLAYS_ACROSS;x++) {
     for (byte y=0;y<DISPLAYS_DOWN;y++)   {
     dmd.drawString(  27+(32*x),  0+(16*y), "C", 2, GRAPHICS_NORMAL );
     dmd.drawString(  27+(32*x),  9+(16*y), "%", 2, GRAPHICS_NORMAL );
     }
     }

     delay(5000);

  dmd.clearScreen(true);
  delay(100);

  //=====================================================Holds date data to display
  String Date = String(nameoftheday[_dtw]) + ", " + String(_day) + "-" + String(month_name[_month-1]) + "-" + String(_year);
  char dt[50];
  Date.toCharArray(dt,50);
  int i=32+10;
  int j=strlen(dt)+(strlen(dt)*5);
//=====================================================

  dmd.selectFont(SystemFont5x7);

  while(1) {
    //_____________________________________________________millis() to display time
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis; //-> save the last time 
      
      //=====================================================Showing the clock in P10
      str_hr_24=String(_hour24);
      str_hr_24.toCharArray(hr_24,3);
  
      if (_hour24<10) {
        dmd.drawString(2, 0, "0", 1, GRAPHICS_NORMAL);
        dmd.drawString(8, 0, hr_24, 1, GRAPHICS_NORMAL);
      }
      else {
        dmd.drawString(2, 0, hr_24, 2, GRAPHICS_NORMAL);
      }
      //=====================================================
      
      //=====================================================Showing ":" in P10
      GetDateTime(); //-> Retrieve time and date data from DS1307
      if (_second %2 == 0) {
        dmd.drawString(14, 0, ":", 2, GRAPHICS_OR);
      }
      else {
        dmd.drawString(14, 0, ":", 2, GRAPHICS_NOR);
      }
      //=====================================================
      
      //=====================================================Showing minutes in P10
      str_mn=String(_minute);
      str_mn.toCharArray(mn,3);
  
      if (_minute<10) {
        dmd.drawString(19, 0, "0", 1, GRAPHICS_NORMAL);
        dmd.drawString(25, 0, mn, 1, GRAPHICS_NORMAL);
      }
      else {
        dmd.drawString(19, 0, mn, 2, GRAPHICS_NORMAL);
      }
      //=====================================================
    }
    //_____________________________________________________

    //_____________________________________________________millis() for display and scrolling date
    unsigned long currentMillis_for_date = millis();
    if (currentMillis_for_date - previousMillis_for_date >= interval_for_date) {
      previousMillis_for_date = currentMillis_for_date; //-> save the last time 
      
      i--;
      dmd.drawString(i, 9, dt, strlen(dt), GRAPHICS_NORMAL);
      if (i<=~j) {
        dmd.clearScreen(true);
        delay(100);
        return;
      }
    }
    //_____________________________________________________
  }
}
