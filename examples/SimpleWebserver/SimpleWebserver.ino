#include <Powerbaas.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>

const char* ssid = "YOUR-SSID";
const char* password = "YOUR-PASSWORD";

Powerbaas powerbaas(true);
MeterReading meterReading;
WebServer server(80);

void setup() {
  Serial.begin(115200);
  setupPowerbaas();
  setupWebserver();
}

void setupPowerbaas() {
  powerbaas.onMeterReading([](const MeterReading& _meterReading) {
    meterReading = _meterReading;
  });
  powerbaas.setup();
}

void setupWebserver() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Start mDNS
  Serial.println("");
  if (MDNS.begin("powerbaas")) {
    Serial.println("Connect to webserver: http://powerbaas.local");
  }
  Serial.print("Connect to webserver: http://");
  Serial.println(WiFi.localIP());

  // Handle index
  server.on("/", []() {
    server.send(200, "text/html", indexHtml());
  });

  server.begin();
}

void loop() {
  powerbaas.receive();
  server.handleClient();
}


String indexHtml() {

  String ptr = R"END(
  <!DOCTYPE html> <html>
  <head>
  <meta name="viewport" content="width=device-width, initial-scale=1.0, user-scalable=no">
  <meta http-equiv="refresh" content="2">
  <title>PowerBaas</title>
  <style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}
    body{margin-top: 20px;} h3 {color: #2d2d2d;margin-bottom: 10px margin-top:50px;}
    .card {display: inline-block;width: 100px;background-color: #3498db;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}
    .card-small{width:40px; display:inline-block;}
    .card-on {background-color: #38b54a;}
    .card-off {background-color: #2d2d2d;}
    .card-action {background-color: #ece50a;}
    p {font-size: 14px;color: #2d2d2d;margin-bottom: 10px;}
  </style>
  </head>
  <body>
  <img style='width:80%; margin-bottom:25px;' src='data:image/png;base64,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'>
  )END";

  // Current power usage
  ptr += "<h3>Current power usage</h3>\n";
  ptr += "<div class=\"card card-on\">" + String(meterReading.powerUsage) + " Watt</div>\n";

  // Power used
  ptr += "<h3>Power used from grid</h3>\n";
  ptr += "<div class=\"card card-off\">" + String(meterReading.powerDeliverHigh) + "</div>\n";
  ptr += "<div class=\"card card-off\">" + String(meterReading.powerDeliverLow) + "</div>\n";

  // Power returned
  ptr += "<h3>Power returned to grid</h3>\n";
  ptr += "<div class=\"card card-off\">" + String(meterReading.powerReturnHigh) + "</div>\n";
  ptr += "<div class=\"card card-off\">" + String(meterReading.powerReturnLow) + "</div>\n";

  // Gas used
  ptr += "<h3>Gas used</h3>\n";
  ptr += "<div class=\"card card-off\">" + String(meterReading.gas) + "</div>\n";

  ptr += "</body>\n";
  ptr += "</html>\n";

  return ptr;
}