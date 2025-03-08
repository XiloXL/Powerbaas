#include <Powerbaas.h>
#include <WiFi.h>
#include <WiFiManager.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>

Powerbaas powerbaas(true);
MeterReading meterReading;
WebServer server(80);

void setup() {
  Serial.begin(115200);

  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    ESP.restart();
  }

  setupWifi();
  setupPowerbaas();
  setupWebserver();
}

void setupWifi() {
  WiFi.mode(WIFI_STA);
  WiFiManager wm;

  bool res = wm.autoConnect("Powerbaas");

  if(!res) {
    Serial.println("Failed to connect");
    ESP.restart();
  }
  else {
    Serial.println("Connected...yeey :)");
  }

  // Start mDNS
  Serial.println("");
  if (MDNS.begin("powerbaas")) {
    Serial.println("Connect to webserver: http://powerbaas.local");
  }
  Serial.print("Connect to webserver: http://");
  Serial.println(WiFi.localIP());
}

void setupPowerbaas() {
  powerbaas.onMeterReading([](const MeterReading& _meterReading) {
    meterReading = _meterReading;
  });
  powerbaas.setup();
}

void setupWebserver() {

  // Handle index
  server.on("/", []() {
    server.send(200, "application/json", statusJson());
  });

  // Reboot
  server.on("/reboot", []() {
    server.send(200, "text/html", "Reboot");
    ESP.restart();
  });

  server.begin();
}

void loop() {
  powerbaas.receive();
  server.handleClient();
}

String statusJson() {
  String json = "{\r\n  \"meterReading\": {";
  json += "\r\n    \"timestamp\": \"" + meterReading.timestamp + "\"";
  json += ",\r\n    \"powerUsage\": " + String(meterReading.powerUsage);
  json += ",\r\n    \"powerDeliverHigh\": " + String(meterReading.powerDeliverHigh);
  json += ",\r\n    \"powerDeliverLow\": " + String(meterReading.powerDeliverLow);
  json += ",\r\n    \"powerReturnHigh\": " + String(meterReading.powerReturnHigh);
  json += ",\r\n    \"powerReturnLow\": " + String(meterReading.powerReturnLow);
  json += ",\r\n    \"gas\": " + String(meterReading.gas);
  json += ",\r\n    \"voltageL1\": " + String(meterReading.voltageL1);
  json += ",\r\n    \"voltageL2\": " + String(meterReading.voltageL2);
  json += ",\r\n    \"voltageL3\": " + String(meterReading.voltageL3);
  json += ",\r\n    \"currentL1\": " + String(meterReading.currentL1);
  json += ",\r\n    \"currentL2\": " + String(meterReading.currentL2);
  json += ",\r\n    \"currentL3\": " + String(meterReading.currentL3);
  json += "\r\n  }";
  json += "\r\n}";

  return json;
}