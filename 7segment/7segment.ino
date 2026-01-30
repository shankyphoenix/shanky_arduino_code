/*
 * Same as HC595Test,  but with leds
 */
#include <HC595.h>

const int chipCount = 1;  // Number of serialy connected 74HC595 (8 maximum)
const int latchPin = 11;  // Pin ST_CP (12) of the 74HC595
const int clockPin = 10;  // Pin SH_CP (11) of the 74HC595
const int dataPin = 12;   // Pin DS (14) of the 74HC595

//Constructor
HC595 ledArray(chipCount,latchPin,clockPin,dataPin);

int delayMs = 1000;
int lastPin = ledArray.lastPin();

void setup() {
  Serial.begin(9600);
  ledArray.reset();
}

void loop() {
   for (int i = 0 ; i <= lastPin ; i++) {
    ledArray.setPin(i, ON);
    Serial.println(i);
    delay(delayMs);
  }
  for (int i = lastPin ; i >= 0 ; i--) {
    ledArray.setPin(i, OFF);
    Serial.println(i);
    delay(delayMs);
  }
}