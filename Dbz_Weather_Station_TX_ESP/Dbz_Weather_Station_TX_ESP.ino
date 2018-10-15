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
float pres_raw;
float pres;
float power;

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
  pres_raw = bmp.readPressure();
  pres = pres_raw / 100;
  power = analogRead(POWER_PIN) * (3.3 / 1023.0);
   
  //Sending values
  msg[0] = temp;
  msg[1] = hum;
  msg[2] = pres;  
  msg[3] = power;
  radio.write(msg, 16);  

  /*
  Serial.print("Temperature: "); 
  Serial.print(temp);
  Serial.println(" *C");
  Serial.print("Humidity: "); 
  Serial.print(hum);
  Serial.println(" %");
  Serial.print("Air pressure: "); 
  Serial.print(pres);
  Serial.println(" hPa"); 
  Serial.print("Power: ");  
  Serial.println(power);  
  Serial.println("**********************");
  */
  
  delay(100);

  ESP.deepSleep(600 * 1000000);
}

void loop() { }
