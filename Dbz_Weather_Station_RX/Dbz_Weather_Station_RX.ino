//Includes
#include <SPI.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include "DHTesp.h"
#include "RF24.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <RtcDS3231.h>

//Defines
#define CE_PIN D4
#define CSN_PIN D8
#define DHT_PIN D3
#define OLED_RESET LED_BUILTIN

//Variables
uint64_t pipe = 0xE8E8F0F0E1LL;
char ssid[] = "Falunet2";
char WifiPassword[] = "19850317";

const int postingInterval = 6; //in every 60 seconds
const int measuringInterval = 10 * 1000; //in every 10 seconds
const int outdoorCheckInterval = 8; //must be transmission in 8 times measuring
int outdoorCheckCounter = 0;
int measuringCounter = 0;
int connectingCounter = 0;
bool isWiFiConnected = false;
bool isOutdoorUnitOK = false;
float msg[4];
float in_temp, in_hum, out_temp, out_hum, pres, rad;
float in_temp_prev, in_hum_prev, out_temp_prev, out_hum_prev, pres_prev, rad_prev;
int year, month, day, hour, minute, minute_prev;

//ETC
Adafruit_SSD1306 display(OLED_RESET);
RtcDS3231<TwoWire> rtc(Wire);
DHTesp dht;
RF24 radio(CE_PIN, CSN_PIN);
WiFiClient client;

void setup() {
  
  Serial.begin(9600);
  dht.setup(DHT_PIN, DHTesp::DHT22);
  delay(dht.getMinimumSamplingPeriod());
  radio.begin();
  radio.openReadingPipe(1,pipe);
  radio.startListening();  
  rtc.Begin();        
  rtc.Enable32kHzPin(false);
  rtc.SetSquareWavePin(DS3231SquareWavePin_ModeNone); 
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setRotation(1);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.clearDisplay();
  display.println("CONNECTING");
  display.println();
  display.println(ssid);
  display.println();
  WiFi.begin(ssid, WifiPassword);
  while ((WiFi.status() != WL_CONNECTED) && (connectingCounter < 10))
  {
    display.print(".");    
    display.display();
    delay(500);
    connectingCounter++;
  }
  if (WiFi.status() == WL_CONNECTED)
    {
      isWiFiConnected = true;
    }
  display.clearDisplay();
}

void loop() {

  CollectingValues();
  if (measuringCounter == postingInterval) 
  {    
    if (isWiFiConnected) SendDatasToWifi();
    measuringCounter = 0;    
  }  
  if (CheckDifferences()) RefreshDisplay();
  delay(measuringInterval);
  measuringCounter++;
  ActualizePrevValues();
}

void CollectingValues() {

  //IN DHT values  
  in_temp = dht.getTemperature();
  in_hum = dht.getHumidity();
  //IN RTC values
  RtcDateTime now = rtc.GetDateTime();
  year = now.Year();
  month = now.Month();
  day = now.Day();
  hour = now.Hour();
  minute = now.Minute();
  
  //OUT values
  if( radio.available()) 
  {
    radio.read( &msg, sizeof(msg) );
    out_temp = msg[0];
    out_hum = msg[1];
    pres = msg[2];    
    rad = msg[3];
    OutdoorTransmissionWasOK();    
  }
  else
    OutdoorTransmissionWasNotOK();
}

void SendDatasToWifi() {

}

bool CheckDifferences() {

  if (  (in_temp_prev  != in_temp)  ||
        (in_hum_prev   != in_hum)   ||
        (out_temp_prev != out_temp) ||
        (out_hum_prev  != out_hum)  ||
        (pres_prev     != pres)     || 
        (rad_prev      != rad)      ||
        (minute_prev   != minute)   )
    return true;    
  else 
    return false;
}

void ActualizePrevValues() {
  
  in_temp_prev  = in_temp;
  in_hum_prev   = in_hum;
  out_temp_prev = out_temp;
  out_hum_prev  = out_hum;
  pres_prev     = pres;
  rad_prev      = rad;
  minute_prev   = minute;  
}

void RefreshDisplay() {

  //Clear the display
  display.clearDisplay();
  
  //Draw the lines
  display.drawLine(0, 0, 63, 0, WHITE);
  display.drawLine(63, 0, 63, 127, WHITE);
  display.drawLine(63, 127, 0, 127, WHITE);
  display.drawLine(0, 127, 0, 0, WHITE);    
  display.drawLine(0, 34, 63, 34, WHITE);
  display.drawLine(0, 65, 63, 65, WHITE);
    
  //Display the numbers
  
  //Hour, Minute
  if (hour < 10)
  {
    if (minute < 10)
    {
      display.setCursor(19, 8);
      display.print(String(hour) + String(":0") + String(minute));
    }
    else
    {
      display.setCursor(19, 8);
      display.print(String(hour) + String(":") + String(minute));
    }    
  }
  else
  {
    if (minute < 10)
    {
      display.setCursor(15, 8);
      display.print(String(hour) + String(":0") + String(minute));
    }
    else
    {
      display.setCursor(15, 8);
      display.print(String(hour) + String(":") + String(minute));
    }    
  }
  
  //Year, Month, Day
  if (month < 10)
  {
    if (day < 10)
    {
      display.setCursor(3, 22);
      display.print(String(year) + String(".0") + String(month) + String(".0") + String(day));
    }
    else
    {
      display.setCursor(3, 22);
      display.print(String(year) + String(".0") + String(month) + String(".") + String(day));
    }
  }
  else
  {
    if (day < 10)
    {
      display.setCursor(3, 22);
      display.print(String(year) + String(".") + String(month) + String(".0") + String(day));
    }
    else
    {
      display.setCursor(3, 22);
      display.print(String(year) + String(".") + String(month) + String(".") + String(day));
    }
  }  
  
  //Indoor weather
  display.setCursor(13, 39);
  display.print("INDOOR");
  display.setCursor(15, 53);
  display.print(String(in_temp, 1) + String(" C"));
  //Outdoor weather
  display.setCursor(11, 70);
  display.print("OUTDOOR");
  if (isOutdoorUnitOK)
  {
    display.setCursor(15, 84);
    display.print(String(out_temp, 1) + String(" C"));
    display.setCursor(15, 98);
    display.print(String(out_hum, 1) + String(" %"));
    if (0.12 < rad)
    {
      display.setCursor(8, 112);
      display.print(String(rad, 2) + String(" uSV"));
    }
    else
    {
      display.setCursor(3, 112);
      display.print(String(pres, 1) + String(" hPa"));
    }
  }
  else
  {
    display.setCursor(9, 98);
    display.print("NO DATA!");
  }  
  display.display();
}

void OutdoorTransmissionWasOK() {
  
  outdoorCheckCounter = 0;
  isOutdoorUnitOK = true;
}

void OutdoorTransmissionWasNotOK() {

  outdoorCheckCounter++;
  if (outdoorCheckCounter > outdoorCheckInterval)
  {
    isOutdoorUnitOK = false;
    outdoorCheckCounter = outdoorCheckInterval +1;
    RefreshDisplay();
  }
}
