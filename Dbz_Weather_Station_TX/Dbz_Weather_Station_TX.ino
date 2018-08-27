//Includes
#include <SPI.h>
#include <Wire.h>
#include "DHT.h"
#include <Adafruit_BMP085.h>
#include "RF24.h"

//Defines
#define DHT_PIN 2
#define CE_PIN 7
#define CSN_PIN 8
#define DHTTYPE DHT22

//Variables
float hum;
float temp;
float pres_raw;
float pres;

float msg[3];
const uint64_t pipe = 0xE8E8F0F0E1LL;

//ETC
DHT dht(DHT_PIN, DHTTYPE);
Adafruit_BMP085 bmp;
RF24 radio(CE_PIN, CSN_PIN);

void setup() {
    
  Serial.begin(9600); 
  dht.begin();
  bmp.begin();
  radio.begin();
  radio.openWritingPipe(pipe);
}

void loop() {

  delay(2000);

  //Collect values
  hum = dht.readHumidity();
  temp = dht.readTemperature();
  pres_raw = bmp.readPressure();
  pres = pres_raw / 100;
  
  //Check DHT values
  if (isnan(hum) || isnan(temp)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  //Check BMP values
  if (isnan(pres)) {
    Serial.println("Failed to read from BMP sensor!");
    return;
  }

  //Sending values
  msg[0] = hum;
  msg[1] = temp;
  msg[2] = pres;  
  radio.write(msg, 12);  
    
  Serial.print("Temperature: "); 
  Serial.print(temp);
  Serial.println(" *C");
  Serial.print("Humidity: "); 
  Serial.print(hum);
  Serial.println(" %");
  Serial.print("Air pressure: "); 
  Serial.print(pres);
  Serial.println(" hPa");  
  
  Serial.println("**********************");

}
