/*
  FibonacciClock.ino
  BlackBug Engineering
  13.12.2020
  https://github.com/dbprof/fibo-clock
*/

#include "FastLED.h"
#include "RTClib.h"

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <Ticker.h>
#include <EEPROM.h>
#include <WiFiUdp.h>
#include "helpers.h"
#include "global.h"
#include "Page_Root.h"
#include "Page_Admin.h"
#include "Page_Script.js.h"
#include "Page_Style.css.h"
#include "Page_NTPsettings.h"
#include "Page_Information.h"
#include "Page_General.h"
#include "PAGE_NetworkConfiguration.h"
#include "example.h"

#define ACCESS_POINT_NAME  "FibonacciClockSetUp"        
#define ACCESS_POINT_PASSWORD  "12345678" 
#define AdminTimeOut 600  // Определяем время, через которое будет закрыт администраторский доступ для безопасности

#define NUM_LEDS 40
#define CLOCK_PIXELS 5
#define MAX_MAMP 2000
#define MAX_VOLT 5
#define LED_PIN D4
#define MAX_COL 6
bool isIncreasing;
int iBrightness, iCurColIndex;
byte oldHours = 0;
byte oldMinutes = 0;
bool isHour;

CRGB cCurCol;
//CRGB arrCurColor[MAX_COL] = {CRGB::Gold, CRGB::Blue, CRGB::Violet, CRGB::Brown, CRGB::Pink, CRGB::Chocolate, CRGB::Green, CRGB::Red, CRGB::Yellow, CRGB::Magenta};
//CRGB arrCurColor[MAX_COL] = {CRGB::Violet,CRGB::Orange, CRGB::Blue, CRGB::Green, CRGB::Red, CRGB::White};
/*
CRGB::Black - черный цвет  - игнорируется
CRGB::White - белый цвет   - игнорируется
CRGB::Red   - красный цвет - часы
CRGB::Green - зеленый цвет - минуты
CRGB::Blue  - синий цвет   - и минуты и часы 
 */
CRGB arrCurColor[MAX_COL] = {CRGB::Black, CRGB::Red, CRGB::Green, CRGB::Blue, CRGB::White};

CRGB leds[NUM_LEDS];

//Время компиляции
//DateTime dtNow = DateTime(__DATE__, __TIME__);

int NextCol() {
  //int i = 0;
  //do {i = random(0, MAX_COL);} 
  //while (i < iIndex);
  //return random(0, MAX_COL);
  iCurColIndex++;
  
  if (iCurColIndex==MAX_COL) iCurColIndex=0;
  
  return iCurColIndex;
}

void resetLEDColor(){
  for(int i=0; i<NUM_LEDS; i++) leds[i]=arrCurColor[0];
}

void changeLEDColor(byte bLED){
  if (isHour) leds[bLED]=arrCurColor[1];
  else{
    if (leds[bLED]==arrCurColor[0]) leds[bLED]=arrCurColor[2];
    else leds[bLED]=arrCurColor[3];
  }
}

void changePixelColor(byte bPixel)
{
  switch(bPixel)
  {
    case 0:
      changeLEDColor(29);
      break;
    case 1:
      changeLEDColor(34);
      break;
    case 2:
      changeLEDColor(30);
      changeLEDColor(31);
      changeLEDColor(32);
      changeLEDColor(33);
      break;
    case 3:
      changeLEDColor(0);
      changeLEDColor(1);
      changeLEDColor(2);

      changeLEDColor(13);
      changeLEDColor(14);
      changeLEDColor(15);

      changeLEDColor(16);
      changeLEDColor(17);
      changeLEDColor(18);
      break;
    case 4:
      changeLEDColor(3);
      changeLEDColor(4);
      changeLEDColor(5);
      changeLEDColor(6);
      changeLEDColor(7);

      changeLEDColor(8);
      changeLEDColor(9);
      changeLEDColor(10);
      changeLEDColor(11);
      changeLEDColor(12);

      changeLEDColor(19);
      changeLEDColor(20);
      changeLEDColor(21);
      changeLEDColor(22);
      changeLEDColor(23);

      changeLEDColor(24);
      changeLEDColor(25);
      changeLEDColor(26);
      changeLEDColor(27);
      changeLEDColor(28);

      changeLEDColor(35);
      changeLEDColor(36);
      changeLEDColor(37);
      changeLEDColor(38);
      changeLEDColor(39);
      break;
  };
}

void changePixels(byte curHM)
{
  switch(curHM)
  {
    case 1:
      switch(random(2))
      {
        case 0:
          changePixelColor(0);
          break;
        case 1:
          changePixelColor(1);
          break;
      }
      break;
    case 2:
      switch(random(2))
      {
        case 0:
          changePixelColor(2);
          break;
        case 1:
          changePixelColor(0);
          changePixelColor(1);
          break;
      }
      break;
    case 3:
      switch(random(3))
      {
        case 0:
          changePixelColor(3);
          break;
        case 1:
          changePixelColor(0);
          changePixelColor(2);
          break;
        case 2:
          changePixelColor(1);
          changePixelColor(2);
          break;
      }
      break;
    case 4:
      switch(random(3))
      {
        case 0:
          changePixelColor(0);
          changePixelColor(3);
          break;
        case 1:
          changePixelColor(1);
          changePixelColor(3);
          break;
        case 2:
          changePixelColor(0);
          changePixelColor(1);
          changePixelColor(2);
          break;
      }
      break;
    case 5:
      switch(random(3))
      {
        case 0:
          changePixelColor(4);
          break;
        case 1:
          changePixelColor(2);
          changePixelColor(3);
          break;
        case 2:
          changePixelColor(0);
          changePixelColor(1);
          changePixelColor(3);
          break;
      }
      break;
    case 6:
      switch(random(4))
      {
        case 0:
          changePixelColor(0);
          changePixelColor(4);
          break;
        case 1:
          changePixelColor(1);
          changePixelColor(4);
          break;
        case 2:
          changePixelColor(0);
          changePixelColor(2);
          changePixelColor(3);
          break;
        case 3:
          changePixelColor(1);
          changePixelColor(2);
          changePixelColor(3);
          break;
      }
      break;
    case 7:
      switch(random(3))
      {
        case 0:
          changePixelColor(2);
          changePixelColor(4);
          break;
        case 1:
          changePixelColor(0);
          changePixelColor(1);
          changePixelColor(4);
          break;
        case 2:
          changePixelColor(0);
          changePixelColor(1);
          changePixelColor(2);
          changePixelColor(3);
          break;
      }
      break;
    case 8:
      switch(random(3))
      {
        case 0:
          changePixelColor(3);
          changePixelColor(4);
          break;
        case 1:
          changePixelColor(0);
          changePixelColor(2);
          changePixelColor(4);
          break;
        case 2:
          changePixelColor(1);
          changePixelColor(2);
          changePixelColor(4);
          break;
      }      
      break;
    case 9:
      switch(random(2))
      {
        case 0:
          changePixelColor(0);
          changePixelColor(3);
          changePixelColor(4);
          break;
        case 1:
          changePixelColor(1);
          changePixelColor(3);
          changePixelColor(4);
          break;
      }      
      break;
    case 10:
      switch(random(2))
      {
        case 0:
          changePixelColor(2);
          changePixelColor(3);
          changePixelColor(4);
          break;
        case 1:
          changePixelColor(0);
          changePixelColor(1);
          changePixelColor(3);
          changePixelColor(4);
          break;
      }            
      break;
    case 11:
      switch(random(2))
      {
        case 0:
          changePixelColor(0);
          changePixelColor(2);
          changePixelColor(3);
          changePixelColor(4);
          break;
        case 1:
          changePixelColor(1);
          changePixelColor(2);
          changePixelColor(3);
          changePixelColor(4); 
          break;
      }          
      break;
    case 12:
      changePixelColor(0);
      changePixelColor(1);
      changePixelColor(2);
      changePixelColor(3);
      changePixelColor(4); 
      break;
  }
}


void setTime(byte curHours, byte curMinutes)
{
  if(oldHours == curHours && oldMinutes/5 == curMinutes/5)
    return;
    
  oldHours = curHours;
  oldMinutes = curMinutes;
  
  //for(int i=0; i<CLOCK_PIXELS; i++)
    //changePixelColor(i, arrCurColor[0]);

  FastLED.clear();
  resetLEDColor();
    
  //setBits(curHours, 0x01);
  //setBits(curMinutes/5, 0x02);
  //for(int i=0; i<CLOCK_PIXELS; i++)
  //{   
  //  setPixel(i, colors[palette][bits[i]]);
  //  strip.show();
  //}

  isHour = true;
  changePixels(curHours);
  isHour = false;
  changePixels(curMinutes/5);

}

void setup() {
  isIncreasing = true;
  iBrightness = 0;
  iCurColIndex = 0;
  FastLED.addLeds<WS2811, LED_PIN, GRB>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setMaxPowerInVoltsAndMilliamps(MAX_VOLT, MAX_MAMP);
  FastLED.setBrightness(iBrightness);
  FastLED.show();

//  /*
  EEPROM.begin(512);
  Serial.begin(115200);
  delay(500);
  Serial.println("Starting ES8266");
  if (!ReadConfig())
  {
    // DEFAULT CONFIG
    config.ssid = "MYSSID";
    config.password = "MYPASSWORD";
    config.dhcp = true;
    config.IP[0] = 192;config.IP[1] = 168;config.IP[2] = 1;config.IP[3] = 100;
    config.Netmask[0] = 255;config.Netmask[1] = 255;config.Netmask[2] = 255;config.Netmask[3] = 0;
    config.Gateway[0] = 192;config.Gateway[1] = 168;config.Gateway[2] = 1;config.Gateway[3] = 1;
    config.ntpServerName = "0.de.pool.ntp.org";
    config.Update_Time_Via_NTP_Every =  0;
    config.timezone = -10;
    config.daylight = true;
    config.DeviceName = "Not Named";
    config.AutoTurnOff = false;
    config.AutoTurnOn = false;
    config.TurnOffHour = 0;
    config.TurnOffMinute = 0;
    config.TurnOnHour = 0;
    config.TurnOnMinute = 0;
    WriteConfig();
    Serial.println("General config applied");
  }
  if (AdminEnabled)
  {
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP( ACCESS_POINT_NAME , ACCESS_POINT_PASSWORD);
  }
  else
  {
    WiFi.mode(WIFI_STA);
  }
  ConfigureWifi();
  server.on ( "/", processExample  );
  server.on ( "/admin/filldynamicdata", filldynamicdata );
  server.on ( "/favicon.ico",   []() { Serial.println("favicon.ico"); server.send ( 200, "text/html", "" );   }  );
  server.on ( "/admin.html", []() { Serial.println("admin.html"); server.send ( 200, "text/html", PAGE_AdminMainPage );   }  );
  server.on ( "/config.html", send_network_configuration_html );
  server.on ( "/info.html", []() { Serial.println("info.html"); server.send ( 200, "text/html", PAGE_Information );   }  );
  server.on ( "/ntp.html", send_NTP_configuration_html  );
  server.on ( "/general.html", send_general_html  );
//  server.on ( "/example.html", []() { server.send ( 200, "text/html", PAGE_EXAMPLE );  } );
  server.on ( "/style.css", []() { Serial.println("style.css"); server.send ( 200, "text/plain", PAGE_Style_css );  } );
  server.on ( "/microajax.js", []() { Serial.println("microajax.js"); server.send ( 200, "text/plain", PAGE_microajax_js );  } );
  server.on ( "/admin/values", send_network_configuration_values_html );
  server.on ( "/admin/connectionstate", send_connection_state_values_html );
  server.on ( "/admin/infovalues", send_information_values_html );
  server.on ( "/admin/ntpvalues", send_NTP_configuration_values_html );
  server.on ( "/admin/generalvalues", send_general_configuration_values_html);
  server.on ( "/admin/devicename",     send_devicename_value_html);
  server.onNotFound ( []() { Serial.println("Page Not Found"); server.send ( 400, "text/html", "Page not Found" );   }  );
  server.begin();
  Serial.println( "HTTP server started" );
  tkSecond.attach(1,Second_Tick);
  UDPNTPClient.begin(2390);  // Port for NTP receive
  
//*/
}

void loop() {
 // /*
  if (AdminEnabled)
  {
    if (AdminTimeOutCounter > AdminTimeOut)
    {
       AdminEnabled = false;
       Serial.println("Admin Mode disabled!");
       WiFi.mode(WIFI_STA);
    }
  }
  if (config.Update_Time_Via_NTP_Every  > 0 )
  {
    if (cNTP_Update > 5 && firstStart)
    {
      NTPRefresh();
      cNTP_Update =0;
      firstStart = false;
    }
    else if ( cNTP_Update > (config.Update_Time_Via_NTP_Every * 60) )
    {

      NTPRefresh();
      cNTP_Update =0;
    }
  }
  if(DateTime.minute != Minute_Old)
  {
     Minute_Old = DateTime.minute;
     if (config.AutoTurnOn)
     {
       if (DateTime.hour == config.TurnOnHour && DateTime.minute == config.TurnOnMinute)
       {
          Serial.println("SwitchON");
       }
     }
     Minute_Old = DateTime.minute;
     if (config.AutoTurnOff)
     {
       if (DateTime.hour == config.TurnOffHour && DateTime.minute == config.TurnOffMinute)
       {
          Serial.println("SwitchOff");
       }
     }
  }
  server.handleClient();
//*/
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  Serial.println("Coloring...");
  for(int i=0; i<NUM_LEDS; i++)
    Serial.println("R=" + String(leds[i].r) + " " + "G=" + String(leds[i].g) + " " + "B=" + String(leds[i].b));
  Serial.println("");
  
  //changePixelColor(0);
  //changePixelColor(1);
  //changePixelColor(2);
  //changePixelColor(3);
  //changePixelColor(4);

  setTime(DateTime.hour%12, DateTime.minute);

  iBrightness = 50;
  FastLED.setBrightness(iBrightness);
  FastLED.show();
  delay(5000);
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///*
  if (Refresh)  
  {
    Refresh = false;
    Serial.println("Refreshing...");
    Serial.printf("FreeMem:%d %d:%d:%d %d.%d.%d \n",ESP.getFreeHeap() , DateTime.hour%12,DateTime.minute, DateTime.second, DateTime.year, DateTime.month, DateTime.day);
  }
//  */
}
