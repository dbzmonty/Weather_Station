//Includes
#include <SPI.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include "DHTesp.h"
#include "RF24.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

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
const int postingInterval = 30; // * 10 seconds
const int measuringInterval = 10 * 1000;
int measuringCounter = 0;
float msg[3];
float in_temp, in_hum, out_temp, out_hum, pres;
float in_temp_changed, in_hum_changed, out_temp_changed, out_hum_changed, pres_changed;

//ETC
Adafruit_SSD1306 display(OLED_RESET);
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
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setRotation(1);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.clearDisplay();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    display.setCursor(5, 5);
    display.print("Connecting...");
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

  //IN values
  in_temp = dht.getTemperature();
  in_hum = dht.getHumidity();
  
  //OUT values
  if( radio.available()) 
  {    
    radio.read( &msg, sizeof(msg) );
    out_temp = msg[0];
    out_hum = msg[1];
    pres = msg[2];    
  }
}

void SendDatasToWifi() {

  if (client.connect(server, 80)) {
        
    // Construct API request body
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
        (pres_changed     != pres) )    
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
  display.drawLine(0, 127, 0, 00, WHITE);  
  display.drawLine(0, 42, 63, 42, WHITE);
  display.drawLine(0, 84, 63, 84, WHITE);
  display.drawLine(31, 0, 31, 84, WHITE);
  
  //Display the numbers
  display.setCursor(5, 15);
  display.print(in_temp, 1);
  display.setCursor(5, 55);
  display.print(in_hum, 1);
  display.setCursor(35,15);
  display.print(out_temp, 1);
  display.setCursor(35,55);
  display.print(out_hum, 1);
  display.setCursor(12,100);
  display.print(pres, 1);
  
  display.display();
}


