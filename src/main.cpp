#include <Arduino.h>
#include <WiFi.h>
#include <LM73.h>
#include "ap_secrets.h"

#define HTTP_PROT

LM73 lm73 = LM73();
WiFiClient client;

#ifdef HTTP_PROT
  #include <HTTPClient.h>
  #include "http_secrets.h"
  const char* serverName = "http://api.thingspeak.com/update";
  String apiKey = SECRET_API_KEY;
  HTTPClient http;
#else
  #include <PubSubClient.h>
  #include "mqtt_secrets.h"
  const long channelID = THINGSPEAK_CHANNEL_NUMBER;
  const char mqttUserName[] = SECRET_MQTT_USERNAME;
  const char mqttPass[] = SECRET_MQTT_PASSWORD;
  const char clientID[] = SECRET_MQTT_CLIENT_ID;
  const long mqttPort = 1883;

  const char* server = "mqtt3.thingspeak.com";
  PubSubClient mqttClient( client );

#endif

const char* ssid = SECRET_SSID;
const char* password = SECRET_PASS;


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

#ifdef HTTP_PROT
void httpRequest(float temp, int ldr){
    http.begin(client, serverName);

    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    String httpRequestData1 = "api_key=" + apiKey + "&field1=" + String(temp) + "&field2=" + String(ldr);
    int status = http.POST(httpRequestData1);
    http.end();

    Serial.printf("POST status: %d\n", status);
}
#else
void connectMqtt(){
  while (!mqttClient.connected())
  {
    if ( mqttClient.connect( clientID, mqttUserName, mqttPass ) ) {
      Serial.print( "MQTT to " );
      Serial.print( server );
      Serial.print (" at port ");
      Serial.print( mqttPort );
      Serial.println( " successful." );
    } else {
      Serial.print( "MQTT connection failed, rc = " );
      // See https://pubsubclient.knolleary.net/api.html#state for the failure code explanation.
      Serial.print( mqttClient.state() );
      Serial.println( " Will try again in a few seconds" );
      delay( 1000 );
    }
  }
}

void mqttPublish(long pubChannelID, String message) {
  String topicString ="channels/" + String( pubChannelID ) + "/publish";
  mqttClient.publish( topicString.c_str(), message.c_str() );
}

#endif


void setup() {
  Serial.begin(9600);
  connectWifi();
  #ifndef HTTP_PROT
    Serial.println("MQTT");
    mqttClient.setServer( server, mqttPort ); 
    connectMqtt();
  #else
    Serial.println("HTTP");
  #endif
  lm73.begin();
}

void loop() {
  static uint32_t last_time;
  if (millis() - last_time > 5000){
    if (WiFi.status() == WL_CONNECTED){
      float val = lm73.getVal(14);
      int ldr_val = analogRead(36);
      Serial.printf("Temp: %f, LDR: %d\n", val, ldr_val);

      #ifdef HTTP_PROT
        httpRequest(val, ldr_val);
      #else
        mqttPublish( channelID, (String("field1=")+String(val)) );
        mqttPublish( channelID, (String("field2=")+String(ldr_val)) );
      #endif
    }
    last_time = millis();
  }
}