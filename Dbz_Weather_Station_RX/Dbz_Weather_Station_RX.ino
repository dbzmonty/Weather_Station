//Includes
#include <SPI.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include "DHTesp.h"
#include "RF24.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <RtcDS3231.h>
#include <ESP8266HTTPClient.h>

//Defines
#define CE_PIN D4
#define CSN_PIN D8
#define DHT_PIN D3
#define OLED_RESET LED_BUILTIN

//Variables
uint64_t pipe = 0xE8E8F0F0E1LL;
char ssid[] = "Falunet2";
char WifiPassword[] = "19850317";
char access_code[] = "14815162342";

const int postingInterval = 600; //in every 10 minutes (600 seconds)
const int measuringInterval = 10 * 1000; //in every 10 seconds
const int outdoorCheckInterval = 30; //must be transmission in 30 times measuring
int outdoorCheckCounter = 0;
int measuringCounter = 0;
int connectingCounter = 0;
bool isWiFiConnected = false;
bool isOutdoorUnitOK = false;
double msg[3];
double in_temp, in_hum, out_temp, out_hum, pres;
double in_temp_prev, in_hum_prev, out_temp_prev, out_hum_prev, pres_prev;
int year, month, day, hour, minute, minute_prev;

//ETC
Adafruit_SSD1306 display(OLED_RESET);
RtcDS3231<TwoWire> rtc(Wire);
DHTesp dht;
RF24 radio(CE_PIN, CSN_PIN);

void setup()
{  
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

void loop()
{
  CollectingValues();
  if (measuringCounter == postingInterval) 
  {
    if (isWiFiConnected && isOutdoorUnitOK) SendDatasToWifi();
    measuringCounter = 0;
  }
  if (CheckDifferences()) RefreshDisplay();
  delay(measuringInterval);
  measuringCounter++;
  ActualizePrevValues();
  //Debugger();
}

void Debugger()
{  
  Serial.print("measuringCounter: ");
  Serial.println(measuringCounter);
  Serial.print("outdoorCheckCounter: ");
  Serial.println(outdoorCheckCounter);  
  Serial.print("isOutdoorUnitOK: ");
  Serial.println(isOutdoorUnitOK);
}

void CollectingValues()
{
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
    OutdoorTransmissionWasOK();
  }
  else
    OutdoorTransmissionWasNotOK();
}

void SendDatasToWifi()
{
  HTTPClient http;
  String uri = "http://something.com/writer.php"; 
  String data = "?access_code=";
  data += access_code;
  data += "&in_temp=";
  data += in_temp;
  data += "&in_hum=";
  data += in_hum;
  data += "&out_temp=";
  data += out_temp;
  data += "&out_hum=";
  data += out_hum;
  data += "&pres=";
  data += pres;
  
  bool httpResult = http.begin(uri + data);
  if (!httpResult)
  {
    Serial.println("Invalid HTTP request:");
    Serial.println(uri + data);
  }
  int httpCode = http.GET();
  if (httpCode > 0) // Request has been made
  {
    Serial.printf("HTTP status: %d\n", httpCode);
    String payload = http.getString();
    Serial.println(payload);
  }
  else // Request could not be made
  {
    Serial.printf("HTTP request failed. Error: %s\r\n", http.errorToString(httpCode).c_str());
  }
  http.end();
}

bool CheckDifferences()
{
  if (  (in_temp_prev  != in_temp)  ||
        (in_hum_prev   != in_hum)   ||
        (out_temp_prev != out_temp) ||
        (out_hum_prev  != out_hum)  ||
        (pres_prev     != pres)     ||
        (minute_prev   != minute)   )
    return true;    
  else 
    return false;
}

void ActualizePrevValues()
{  
  in_temp_prev  = in_temp;
  in_hum_prev   = in_hum;
  out_temp_prev = out_temp;
  out_hum_prev  = out_hum;
  pres_prev     = pres;
  minute_prev   = minute;  
}

void RefreshDisplay()
{
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
    display.setCursor(3, 112);
    display.print(String(pres, 1) + String(" hPa"));    
  }
  else
  {
    display.setCursor(9, 98);
    display.print("NO DATA!");
  }  
  display.display();
}

void OutdoorTransmissionWasOK()
{  
  outdoorCheckCounter = 0;
  isOutdoorUnitOK = true;
}

void OutdoorTransmissionWasNotOK()
{
  outdoorCheckCounter++;
  if (outdoorCheckCounter > outdoorCheckInterval)
  {
    isOutdoorUnitOK = false;
    outdoorCheckCounter = outdoorCheckInterval +1;
    RefreshDisplay();
  }
}
