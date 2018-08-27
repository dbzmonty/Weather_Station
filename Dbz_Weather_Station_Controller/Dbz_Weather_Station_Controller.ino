#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET LED_BUILTIN
Adafruit_SSD1306 display(OLED_RESET);

float in_temp = 23.5;
float in_hum = 65.8;
float out_temp = 31.9;
float out_hum = 49.8;
float pres = 1000.9;

void setup() {
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
  
  //Display the numbers
  display.setCursor(3, 15);
  display.print(in_temp);
  display.setCursor(3, 55);
  display.print(in_hum);
  display.setCursor(33,15);
  display.print(out_temp);
  display.setCursor(33,55);
  display.print(out_hum);
  display.setCursor(10,100);
  display.print(pres);
  
  display.display();
  delay(1000);
  
}
