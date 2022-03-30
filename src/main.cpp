#include <Arduino.h>
#include "ThingSpeak.h"
#include <LM73.h>
LM73 lm73 = LM73();
void setup() {
  // put your setup code here, to run once:
  lm73.begin();
  Serial.begin(9600);
}

void loop() {
  float val = lm73.getVal(14);
  Serial.println(val);
}