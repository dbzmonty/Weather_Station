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
double msg[3];
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
     
  //Sending values
  msg[0] = dht.getTemperature();
  msg[1] = dht.getHumidity();
  msg[2] = getPres();
  radio.write(msg, 24);
  
  delay(100);
  ESP.deepSleep(60 * 1000000);
}

double getPres() {
  double raw = bmp.readPressure();
  return raw / 100; 
}

void loop() { }
