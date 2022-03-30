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
  int ldr_val = analogRead(36);
  Serial.printf("Temp: %f, LDR: %d\n", val, ldr_val);
}