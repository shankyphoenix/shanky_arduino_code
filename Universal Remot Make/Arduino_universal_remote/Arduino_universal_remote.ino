#include <IRremote.h>
#include "LowPower.h"

IRsend irsend;

const int b1  = 2;
const int b2  = 4;
const int b3  = 5;
const int b4  = 6;
const int b5  = 7;
const int b6  = 13;

int timer;
int modeCounter = 0;

void wakeUp() {
  timer = 0;
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(b1, INPUT);
  pinMode(b2, INPUT);
  pinMode(b3, INPUT);
  pinMode(b4, INPUT);
  pinMode(b5, INPUT);
  pinMode(b6, INPUT);
}

void loop() {
  attachInterrupt(0, wakeUp, HIGH);
  while (timer < 10000) {
    if (digitalRead(b1) == HIGH) {
      timer = 0;
      delay(50);
      irsend.sendNEC(0xA90, 32);
    }

    if (digitalRead(b2) == HIGH) {
      timer = 0;
      delay(50);
      irsend.sendNEC(0xC90, 32);
    }

    if (digitalRead(b3) == HIGH) {
      timer = 0;
      delay(50);
      irsend.sendNEC(0x90, 32);
    }

    if (digitalRead(b4) == HIGH) {
      timer = 0;
      delay(50);
      irsend.sendNEC(0x890, 32);
    }

    if (digitalRead(b5) == HIGH) {
      timer = 0;
      delay(50);
      irsend.sendNEC(0xEFE421, 32);
    }
    delay(1);
    timer = timer + 1;

  }
  
  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
}
