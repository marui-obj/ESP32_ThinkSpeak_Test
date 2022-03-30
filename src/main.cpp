#include <Arduino.h>
#include "ThingSpeak.h"
#include <LM73.h>
#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "รักคนอ่าน";
const char* password = "12345678";

const char* serverName = "http://api.thingspeak.com/update";

String apiKey = "API key here";

LM73 lm73 = LM73();
void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  lm73.begin();
}

void loop() {
  static uint16_t last_time;
  if (millis() - last_time > 5000){
    if (WiFi.status() == WL_CONNECTED){
      WiFiClient client;
      HTTPClient http;

      http.begin(client, serverName);
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      float val = lm73.getVal(14);
      int ldr_val = analogRead(36);
      Serial.printf("Temp: %f, LDR: %d\n", val, ldr_val);
      String httpRequestData = "api_key=" + apiKey + "&field1=" + "1";
      int httpResponseCode = http.POST(httpRequestData);
      http.end();
    }
    last_time = millis();
  }
}