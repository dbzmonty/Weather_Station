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
float in_temp;
float in_hum;
float out_temp;
float out_hum;
float pres;

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

  //Draw the lines
  display.drawLine(0, 0, 63, 0, WHITE);
  display.drawLine(63, 0, 63, 127, WHITE);
  display.drawLine(63, 127, 0, 127, WHITE);
  display.drawLine(0, 127, 0, 00, WHITE);  
  display.drawLine(0, 42, 63, 42, WHITE);
  display.drawLine(0, 84, 63, 84, WHITE);
  display.drawLine(31, 0, 31, 84, WHITE);
  display.display();

  if( radio.available())
  {
    while (radio.available())
    {
      radio.read( &msg, sizeof(msg) );
      
      in_temp = dht.getTemperature();
      in_hum = dht.getHumidity();
      out_temp = msg[0];
      out_hum = msg[1];
      pres = msg[2];      
      
      //IN Temp
      Serial.print("IN Temperature: ");
      Serial.print(in_temp, 1);
      Serial.println(" °C");
      //IN Hum
      Serial.print("IN Humidity: ");
      Serial.print(in_hum, 1);
      Serial.println(" %");
      //OUT Temp
      Serial.print("OUT Temperature: ");
      Serial.print(out_temp, 1);
      Serial.println(" °C");
      //OUT Hum
      Serial.print("OUT Humidity: ");
      Serial.print(out_hum, 1);
      Serial.println(" %");
      //PRES
      Serial.print("Air Pressure: ");
      Serial.print(pres, 1);
      Serial.println(" hPa");
      Serial.println("**********************");

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
  }  
  delay(100);
}
