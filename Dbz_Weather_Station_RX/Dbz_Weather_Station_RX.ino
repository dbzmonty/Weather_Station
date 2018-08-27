//Includes
#include <SPI.h>
#include "DHT.h"
#include "RF24.h"

//Defines
#define CE_PIN 7
#define CSN_PIN 8
#define DHT_PIN 2
#define DHTTYPE DHT22

//Variables
const uint64_t pipe = 0xE8E8F0F0E1LL;
float msg[3];
float in_temp;
float in_hum;
float out_temp;
float out_hum;
float pres;


//ETC
DHT dht(DHT_PIN, DHTTYPE);
RF24 radio(CE_PIN, CSN_PIN);

void setup()
{
  Serial.begin(9600);
  dht.begin();
  radio.begin();
  radio.openReadingPipe(1,pipe);
  radio.startListening();
}

void loop()
{
  if( radio.available())
  {
    while (radio.available())
    {
      radio.read( &msg, sizeof(msg) );
      
      in_temp = dht.readTemperature();
      in_hum = dht.readHumidity();  
      out_temp = msg[0];
      out_hum = msg[1];
      pres = msg[2];      
      
      //IN Temp
      Serial.print("IN Temperature: ");
      Serial.print(in_temp);
      Serial.println(" °C");
      //IN Hum
      Serial.print("IN Humidity: ");
      Serial.print(in_hum);
      Serial.println(" %");
      //OUT Temp
      Serial.print("OUT Temperature: ");
      Serial.print(out_temp);
      Serial.println(" °C");
      //OUT Hum
      Serial.print("OUT Humidity: ");
      Serial.print(out_hum);
      Serial.println(" %");
      //PRES
      Serial.print("Air Pressure: ");
      Serial.print(pres);
      Serial.println(" hPa");
      Serial.println("**********************");
    }
  }  
  delay(100);
}
