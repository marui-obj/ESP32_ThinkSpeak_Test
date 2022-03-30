#include <Arduino.h>
#include "ThingSpeak.h"
#include <LM73.h>
#include <WiFi.h>

#define HTTP_PROT

#ifdef HTTP_PROT
  #include <HTTPClient.h>
  const char* serverName = "http://api.thingspeak.com/update";
  String apiKey = "6QLLOBS0QJTAEYOF";
#else
  #include <PubSubClient.h>
  #include "mqtt_secrets.h"
  PubSubClient MQTT_CLIENT;
  const char* mqttUserName = "CAYNEgc8EicmJAkfIhYqEB8";
  const char* mqttPass = "uITNYgH+9pwrbQQy1J+mpjRI";
  const char* clientID = "CAYNEgc8EicmJAkfIhYqEB8";
  const char* mqttserver = "mqtt.thingspeak.com";
#endif

const char* ssid = "รักคนอ่าน";
const char* password = "12345678";


LM73 lm73 = LM73();

void connectWifi(){
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
}

void http(float temp, int ldr){
  WiFiClient client;
  HTTPClient http;

  http.begin(client, serverName);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  String httpRequestData1 = "api_key=" + apiKey + "&field1=" + String(temp);
  String httpRequestData2 = "api_key=" + apiKey + "&field2=" + String(ldr);
  http.POST(httpRequestData1);
  http.POST(httpRequestData2);
  http.end();
}
void setup() {
  connectWifi();
  Serial.begin(9600);
  lm73.begin();
}

void loop() {
  static uint16_t last_time;
  if (millis() - last_time > 5000){
    if (WiFi.status() == WL_CONNECTED){
      float val = lm73.getVal(14);
      int ldr_val = analogRead(36);
      Serial.printf("Temp: %f, LDR: %d\n", val, ldr_val);

      #ifdef HTTP_PROT
        http(val, ldr_val);
      #else
        //mqtt
      #endif
    }
    last_time = millis();
  }
}