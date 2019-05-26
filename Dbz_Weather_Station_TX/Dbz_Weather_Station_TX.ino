//Includes
#include <SPI.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include "DHTesp.h"
#include <Adafruit_BMP085.h>
#include "RF24.h"

//Defines
#define CE_PIN D4
#define CSN_PIN D8
#define DHT_PIN D3
#define POWER_PIN A0

//Variables
float temp;
float hum;
float pres;
float rad;

float msg[4];
const uint64_t pipe = 0xE8E8F0F0E1LL;

//ETC
DHTesp dht;
Adafruit_BMP085 bmp;
RF24 radio(CE_PIN, CSN_PIN);

void setup() {
    
  Serial.begin(9600); 
  WiFi.mode(WIFI_OFF);
  WiFi.forceSleepBegin();
  pinMode(POWER_PIN,INPUT);
  dht.setup(DHT_PIN, DHTesp::DHT22);
  delay(dht.getMinimumSamplingPeriod());
  bmp.begin();
  radio.begin();
  radio.openWritingPipe(pipe);  
  delay(500);
  
  //Collect values
  temp = dht.getTemperature();
  hum = dht.getHumidity();
  pres = bmp.readPressure() / 100;
  rad = getRadiation();
   
  //Sending values
  msg[0] = temp;
  msg[1] = hum;
  msg[2] = pres;
  msg[3] = rad;
  radio.write(msg, 16);
  
  delay(100);
  ESP.deepSleep(60 * 1000000);
}

float getRadiation() {  
  return 0.12;
}

void loop() { }
