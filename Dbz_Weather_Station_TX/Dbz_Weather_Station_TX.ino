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
double temp_temp;
double temp_hum;
double temp_pres;

double temp;
double hum;
double pres;

int measuringCounter;
int measuringInterval;
bool firstStart;

double msg[3];
const uint64_t pipe = 0xE8E8F0F0E1LL;

//ETC
DHTesp dht;
Adafruit_BMP085 bmp;
RF24 radio(CE_PIN, CSN_PIN);

void setup() {    
  //Serial.begin(9600); 
  WiFi.mode(WIFI_OFF);
  WiFi.forceSleepBegin();
  pinMode(POWER_PIN, INPUT);
  dht.setup(DHT_PIN, DHTesp::DHT22);
  delay(dht.getMinimumSamplingPeriod());
  bmp.begin();
  radio.begin();
  radio.openWritingPipe(pipe);
  delay(500);
  measuringCounter = 0;
  measuringInterval = 10 * 1000; // Wait 10 seconds
  firstStart = true;
}

void loop() { 
  countAverage();  
  //displayValues();
  
  if (measuringCounter == 8) {    
    sendValues();
    measuringCounter = 0;
  }
  else measuringCounter++;

  delay(measuringInterval);
}

double getPres() {
  double raw = bmp.readPressure();
  return raw / 100; 
}

void countAverage() {
  temp_temp = dht.getTemperature();
  temp_hum = dht.getHumidity();
  temp_pres = getPres();

  if (firstStart) {
    temp = temp_temp;
    hum = temp_hum;
    pres = temp_pres;
    firstStart = false;
  }
  else {
    temp = (temp + temp_temp) / 2;
    hum = (hum + temp_hum) / 2;
    pres = (pres + temp_pres) / 2;
  }  
}

void sendValues() {
  msg[0] = temp;
  msg[1] = hum;
  msg[2] = pres;
  radio.write(msg, 24);
}

void displayValues() {
  Serial.print("measuringCounter: ");
  Serial.println(measuringCounter);
  Serial.println("TEMP:");
  Serial.println(temp_temp);
  Serial.println(temp_hum);
  Serial.println(temp_pres);
  Serial.println("AVG:");
  Serial.println(temp);
  Serial.println(hum);
  Serial.println(pres);
  Serial.println("###############################");
}
