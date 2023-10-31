#include <Wire.h>              // include Wire library (required for I2C devices)
#include <Adafruit_GFX.h>      // include Adafruit graphics library
#include <Adafruit_PCD8544.h>  // include Adafruit PCD8544 (Nokia 5110) library
#include <RTClib.h>            // include Adafruit RTC library
 
// Nokia 5110 LCD module connections (CLK - D4, DIN - D3, D/C - D2, CS - D1, RST - D0)
Adafruit_PCD8544 display = Adafruit_PCD8544(2, 0, 4, 5, 16);
 
// initialize RTC library
RTC_DS3231 rtc;
DateTime   now;
 
const int button1 = 13;  // button B1 is connected to NodeMCU D7
const int button2 = 15;  // button B2 is connected to NodeMCU D8
 
void setup(void)
{
  pinMode(button1, INPUT);
  pinMode(button2, INPUT);
 
  // initialize the display
  display.begin();
 
  // you can change the contrast around to adapt the display
  // for the best viewing!
  display.setContrast(50);
 
  display.clearDisplay();   // clear the screen and buffer
  display.setTextSize(1);
  display.setTextColor(BLACK, WHITE);
  display.setCursor(28, 16);
  display.print("TIME:");
  display.setCursor(13, 32);
  display.print("CHIP TEMP:");
  display.display();
 
  Wire.begin(12, 14);  // set I2C pins [SDA = D6, SCL = D5], default clock is 100kHz
  rtc.begin();         // initialize RTC chip
}
 
// a small function for button1 (B1) debounce
bool debounce ()
{
  byte count = 0;
  for(byte i = 0; i < 5; i++)
  {
    if (digitalRead(button1))
      count++;
    delay(10);
  }
 
  if(count > 2)  return 1;
  else           return 0;
}
 
void RTC_display()
{
  char dow_matrix[7][10] = {"SUNDAY", "MONDAY", "TUESDAY", "WEDNESDAY", "THURSDAY", "FRIDAY", "SATURDAY"};
  byte x_pos[7] = {24, 24, 21, 15, 18, 24, 18};
  static byte previous_dow = 8;
 
  // print day of the week
  if( previous_dow != now.dayOfTheWeek() )
  {
    previous_dow = now.dayOfTheWeek();
    display.fillRect(15, 0, 54, 8, WHITE);     // draw rectangle (erase day from the display)
    display.setCursor(x_pos[previous_dow], 0);
    display.print( dow_matrix[now.dayOfTheWeek()] );
  }
 
  // print date
  display.setCursor(12, 8);
  display.printf( "%02u-%02u-%04u", now.day(), now.month(), now.year() );
  // print time
  display.setCursor(18, 24);
  display.printf( "%02u:%02u:%02u", now.hour(), now.minute(), now.second() );
  // now update the screen
  display.display();
}
 
byte edit(byte parameter)
{
  static byte i = 0, y_pos,
              x_pos[5] = {12, 30, 60, 18, 36};
  if(i < 3)
    y_pos = 8;
  else
    y_pos = 24;
 
  while( debounce() );   // call debounce function (wait for B1 to be released)
 
  while(true) {
    while(digitalRead(button2)) {  // while B2 is pressed
      parameter++;
      if(i == 0 && parameter > 31)    // if day > 31 ==> day = 1
        parameter = 1;
      if(i == 1 && parameter > 12)    // if month > 12 ==> month = 1
        parameter = 1;
      if(i == 2 && parameter > 99)    // if year > 99 ==> year = 0
        parameter = 0;
      if(i == 3 && parameter > 23)    // if hours > 23 ==> hours = 0
        parameter = 0;
      if(i == 4 && parameter > 59)    // if minutes > 59 ==> minutes = 0
        parameter = 0;
 
      display.setCursor(x_pos[i], y_pos);
      display.printf("%02u", parameter);
      display.display();  // update the screen
      delay(200);         // wait 200ms
    }
 
    display.fillRect(x_pos[i], y_pos, 11, 8, WHITE);
    display.display();  // update the screen
    unsigned long previous_m = millis();
    while( (millis() - previous_m < 250) && !digitalRead(button1) && !digitalRead(button2)) ;
    display.setCursor(x_pos[i], y_pos);
    display.printf("%02u", parameter);
    display.display();  // update the screen
    previous_m = millis();
    while( (millis() - previous_m < 250) && !digitalRead(button1) && !digitalRead(button2)) ;
 
    if(digitalRead(button1))
    {                     // if button B1 is pressed
      i = (i + 1) % 5;    // increment 'i' for the next parameter
      return parameter;   // return parameter value and exit
    }
  }
}
 
// main loop
void loop()
{
  if(digitalRead(button1))  // if B1 is pressed
  if( debounce() )          // call debounce function (make sure B1 is pressed)
  {
    while( debounce() );  // call debounce function (wait for B1 to be released)
 
    byte day    = edit( now.day() );          // edit date
    byte month  = edit( now.month() );        // edit month
    byte year   = edit( now.year() - 2000 );  // edit year
    byte hour   = edit( now.hour() );         // edit hours
    byte minute = edit( now.minute() );       // edit minutes
 
    // write time & date data to the RTC chip
    rtc.adjust(DateTime(2000 + year, month, day, hour, minute, 0));
 
    while(debounce());  // call debounce function (wait for button B1 to be released)
  }
 
  now = rtc.now();  // read current time and date from the RTC chip
  RTC_display();    // display time & calendar
 
  // read chip temperature
  Wire.beginTransmission(0x68); // start I2C protocol with DS3231 address
  Wire.write(0x11);             // send register address (temperature MSB)
  Wire.endTransmission(false);  // I2C restart
  Wire.requestFrom(0x68, 2);    // request 2 bytes from DS3231 and release I2C bus at end of reading
  byte t_msb = Wire.read();     // read temperature MSB
  byte t_lsb = Wire.read();     // read temperature LSB
 
  // print chip temperature
  uint16_t chip_temp = (uint16_t)t_msb << 2 | t_lsb >> 6;
  display.setCursor(15, 40);
  if(t_msb & 0x80) {
    chip_temp |= 0xFC00;
    display.printf( "-%02u.%02u C", abs((int)chip_temp * 25) / 100, abs((int)chip_temp * 25) % 100);
  }
  else
    display.printf( " %02u.%02u C", (chip_temp * 25) / 100, (chip_temp * 25) % 100);
  display.drawRect(53, 40, 3, 3, BLACK); // print degree symbol ( ° )
 
  RTC_display();
  delay(100);     // wait 100ms
}
