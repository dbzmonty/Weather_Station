//Includes
#include <SPI.h>
#include <Wire.h>
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
float msg[3];
float in_temp, in_hum, out_temp, out_hum, pres;
float in_temp_changed, in_hum_changed, out_temp_changed, out_hum_changed, pres_changed;

//ETC
Adafruit_SSD1306 display(OLED_RESET);
DHTesp dht;
RF24 radio(CE_PIN, CSN_PIN);

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
}

void loop() {

  CollectingValues();
  SendDatasToWifi();
  if ( CheckDifferences() ) { RefreshDisplay(); }
  delay(2000);
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


