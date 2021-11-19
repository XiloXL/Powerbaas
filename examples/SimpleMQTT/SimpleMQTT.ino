#include <Powerbaas.h>
#include <WiFi.h>
#include <WiFiClient.h>

#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

const char* ssid = "";
const char* password = "";

/**
 * This example is based on the AdaFruit MQTT client which should be installed
 * for the code to be compiled. There should be a working MQTT broker available.
 * 
 */ 

#define MQTT_SERVER      ""   // ex. hostname.com
#define MQTT_SERVERPORT  1883 // use 8883 for SSL
#define MQTT_USERNAME    ""
#define MQTT_KEY         ""

WiFiClient client;

Adafruit_MQTT_Client mqtt(&client, MQTT_SERVER, MQTT_SERVERPORT, MQTT_USERNAME, MQTT_KEY);

Adafruit_MQTT_Publish telegramChannel = Adafruit_MQTT_Publish(&mqtt, "telegram/usage");

Powerbaas powerbaas(true);
MeterReading meterReading;

void setup() {
  Serial.begin(115200);
  setupPowerbaas();
}

int lastUpdateMilis = 0;
int currentMilis = 0;

void loop() {
  MQTT_connect();
  
  powerbaas.receive();
  
  currentMilis = millis();
  if (currentMilis - lastUpdateMilis > 1000) {
    Serial.println("MQTT");
    
    telegramChannel.publish(meterReading.powerUsage);  
    
    lastUpdateMilis = currentMilis;
  }
}

void setupPowerbaas() {
  powerbaas.onMeterReading([](const MeterReading& _meterReading) {
    meterReading = _meterReading;
  });
  powerbaas.setup();
}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}


