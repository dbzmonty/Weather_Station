//Includes
#include <SPI.h>
#include "DHT.h"
#include "RF24.h"

//Defines
#define CE_PIN 7
#define CSN_PIN 8

//Variables
const uint64_t pipe = 0xE8E8F0F0E1LL;
float msg[4];

//ETC
RF24 radio(CE_PIN, CSN_PIN);

void setup()
{
  Serial.begin(9600);
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
      
      Serial.print("Humidity: "); 
      Serial.print(msg[0]);
      Serial.println(" %");
      Serial.print("Temperature: "); 
      Serial.print(msg[1]);
      Serial.println(" *C");
    
      Serial.print("Luminosity: "); 
      Serial.print(msg[2]);
      Serial.println(" %");
      Serial.print("Rain intensity: "); 
      Serial.print(msg[3]);
      Serial.println(" %");
          
      Serial.println("**********************");
    }
  }  
  delay(100);
}
