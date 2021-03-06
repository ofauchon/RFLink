#ifdef ESP32
#include <WiFi.h>
#elif ESP8266
#include <ESP8266WiFi.h>
#endif

void setup_WIFI_OFF() {
  WiFi.persistent(false);
  WiFi.setAutoReconnect(false);
  WiFi.setSleepMode(WIFI_MODEM_SLEEP);
  WiFi.mode(WIFI_OFF);
  WiFi.forceSleepBegin();
}

#if defined(MQTT_ACTIVATED) && (defined(ESP32) || defined(ESP8266))
#include <PubSubClient.h>
#include "6_Credentials.h"

// Update these with values suitable for your network.

WiFiClient WIFIClient;
PubSubClient MQTTClient; // MQTTClient(WIFIClient);

void setup_WIFI() {
  WiFi.persistent(false);
  WiFi.setAutoReconnect(true);
  WiFi.setSleepMode(WIFI_MODEM_SLEEP);
  WiFi.mode(WIFI_STA);

  // Comment out for Dynamic IP
  WiFi.config(ip, gateway, subnet);

  // We start by connecting to a WiFi network
  Serial.print(F("\nConnecting to "));
  Serial.print(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.print(F("\nWiFi connected\t"));
  Serial.print(F("IP address: "));
  Serial.print(WiFi.localIP());
  Serial.print(F("\tRSSI "));
  Serial.println(WiFi.RSSI());
}

void setup_MQTT() {
  MQTTClient.setClient(WIFIClient);
  MQTTClient.setServer(MQTT_SERVER, MQTT_PORT);
  // MQTTClient.setCallback(callback);
}

/*
  void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print(F("Message arrived ["));
  Serial.print(topic);
  Serial.print("] ");
  for (unsigned int i = 0; i < length; i++) {
    Serial.write(payload[i]);
  }
  Serial.write('\n');
  Serial.println();
  }
*/

void reconnect() {
  // Loop until we're reconnected
  delay(1);
  while (!MQTTClient.connected()) {
    Serial.print(F("Attempting MQTT connection..."));
    // Attempt to connect
    if (MQTTClient.connect(MQTT_ID, MQTT_USER, MQTT_PSWD))
    {
      Serial.println(F("Connected"));
      // Once connected, resubscribe
      // MQTTClient.subscribe(MQTT_TOPIC_IN);
    }
    else
    {
      Serial.print(F("\nFailed, rc="));
      Serial.print(MQTTClient.state());
      Serial.println(F("\tTry again in 5 seconds"));
      // Wait 5 seconds before retrying
      for (byte i = 0; i < 10; i++) delay(500); // delay(5000) may cause hang
    }
  }
}

void publishMsg() {
  if (MQTTbuffer[0] != 0)
  {
    if (!MQTTClient.connected()) {
      reconnect();
    }
    // MQTTClient.loop();

    MQTTClient.publish(MQTT_TOPIC_OUT, MQTTbuffer);
    MQTTbuffer[0] = 0;
  }
}

#endif
