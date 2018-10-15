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
const uint64_t pipe = 0xE8E8F0F0E1LL;
const char* ssid = "Falunet2";
const char* password = "19850317";
const int channelID = 574312;
String writeAPIKey = "WTLF6EM99QTS05R6";
const char* server = "api.thingspeak.com";
const int postingInterval = 60; // * 10 seconds
const int measuringInterval = 10 * 1000;
int measuringCounter = 0;
float msg[4];
float in_temp, in_hum, out_temp, out_hum, pres, power;
float in_temp_changed, in_hum_changed, out_temp_changed, out_hum_changed, pres_changed, power_changed;
int year, month, day, hour, minute, minute_changed;


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
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {    
    display.println("CONNECTING");
    display.println("TO");
    display.println(ssid);
    display.display();
    delay(500);
  }
  display.clearDisplay();
}

void loop() {

  CollectingValues();
  if ( measuringCounter == postingInterval ) 
  {
    SendDatasToWifi(); 
    measuringCounter = 0;
  }
  if ( CheckDifferences() ) { RefreshDisplay(); }
  delay(measuringInterval);
  measuringCounter++;
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
    power = msg[3];
  }
}

void SendDatasToWifi() {

  if (client.connect(server, 80)) {
        
    String body = "field1=";
           body += String(in_temp);
           body += "&field2=";
           body += String(in_hum);
           body += "&field3=";
           body += String(out_temp);
           body += "&field4=";
           body += String(out_hum);
           body += "&field5=";
           body += String(pres);
           body += "&field6=";
           body += String(power);

    client.println("POST /update HTTP/1.1");
    client.println("Host: api.thingspeak.com");
    client.println("User-Agent: ESP8266 (nothans)/1.0");
    client.println("Connection: close");
    client.println("X-THINGSPEAKAPIKEY: " + writeAPIKey);
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.println("Content-Length: " + String(body.length()));
    client.println("");
    client.print(body);
  }
  client.stop();    
}

boolean CheckDifferences() {

  if (  (in_temp_changed  != in_temp)  ||
        (in_hum_changed   != in_hum)   ||
        (out_temp_changed != out_temp) ||
        (out_hum_changed  != out_hum)  ||
        (pres_changed     != pres)     || 
        (power_changed    != power)    ||
        (minute_changed   != minute)   )
    return true;    
  else 
    return false;
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
  
  //Weather
  display.setCursor(13, 39);
  display.print("INDOOR");
  display.setCursor(15, 53);
  display.print(String(in_temp, 1) + String(" C"));
  display.setCursor(11, 70);
  display.print("OUTDOOR");
  display.setCursor(15, 84);
  display.print(String(out_temp, 1) + String(" C"));
  display.setCursor(15, 98);
  display.print(String(out_hum, 1) + String(" %"));
  if ((0.0 < power) && (power < 3.3))
  {
    display.setCursor(11, 112);
    display.print("BATTERY");
  }
  else
  {
    display.setCursor(3, 112);
    display.print(String(pres, 1) + String(" hPa"));
  }
  display.display();
}
