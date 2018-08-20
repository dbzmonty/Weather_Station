//Includes
#include <SPI.h>
#include "DHT.h"
#include "RF24.h"

//Defines
#define DHT_PIN 2
#define LUM_PIN A0
#define RAIN_PIN A1
#define CE_PIN 7
#define CSN_PIN 8
#define DHTTYPE DHT22

//Variables
float hum;
float temp;
float lum_raw;
float rain_raw;
float lum_inten;
float rain_inten;

float msg[4];
const uint64_t pipe = 0xE8E8F0F0E1LL;

//ETC
DHT dht(DHT_PIN, DHTTYPE);
RF24 radio(CE_PIN, CSN_PIN);

void setup() {
    
  Serial.begin(9600); 
  dht.begin();
  radio.begin();
  radio.openWritingPipe(pipe);
}

void loop() {

  delay(2000);

  //Collect values
  hum = dht.readHumidity();
  temp = dht.readTemperature();
  lum_raw = analogRead(LUM_PIN);
  lum_inten = map(lum_raw, 0, 1023, 0, 100);
  rain_raw = 1024 - analogRead(RAIN_PIN);
  if (rain_raw < 400) {
    rain_inten = 0;
  }
  else {
    rain_inten = map(rain_raw, 400, 700, 0, 100); 
  }
  
  //Check DHT values
  if (isnan(hum) || isnan(temp)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  //Check analog values
  if (isnan(lum_inten) || isnan(rain_inten)) {
    Serial.println("Failed to read from analog sensors!");
    return;
  }

  //Sending values
  msg[0] = hum;
  msg[1] = temp;
  msg[2] = lum_inten;
  msg[3] = rain_inten;
  radio.write(msg, 16);  
    
  Serial.print("Humidity: "); 
  Serial.print(hum);
  Serial.println(" %");
  Serial.print("Temperature: "); 
  Serial.print(temp);
  Serial.println(" *C");

  Serial.print("Luminosity: "); 
  Serial.print(lum_inten);
  Serial.println(" %");
  Serial.print("Rain intensity: "); 
  Serial.print(rain_inten);
  Serial.println(" %");
  
  Serial.println("**********************");

}
