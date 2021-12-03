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

  // Head and styling
  String ptr = R"END(
    <!DOCTYPE html> <html>
    <head>
    <meta name="viewport" content="width=device-width, initial-scale=1.0, user-scalable=no">
    <meta http-equiv="refresh" content="1">
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
    <a href="/">
    <img style='border:none; width:80%; margin-bottom:25px;' src="https://t.eu1.jwwb.nl/W1292300/7Z67ppdpu6A8hHiS3s7fsxO1-ys=/658x0/filters:quality(70)/f.eu1.jwwb.nl%2Fpublic%2Fs%2Fq%2Fx%2Ftemp-ubafshmstbomdhzsimxh%2Fum7ocq%2Fimage-1.png" />
    </a>
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

  // Voltage per phase
  ptr += "<h3>Voltage per phase</h3>\n";
  ptr += "<div class=\"card card-off\">" + String(meterReading.voltageL1) + "</div>\n";
  ptr += "<div class=\"card card-off\">" + String(meterReading.voltageL2) + "</div>\n";
  ptr += "<div class=\"card card-off\">" + String(meterReading.voltageL3) + "</div>\n";

  // Current per phase
  ptr += "<h3>Current per phase</h3>\n";
  ptr += "<div class=\"card card-off\">" + String(meterReading.currentL1) + "</div>\n";
  ptr += "<div class=\"card card-off\">" + String(meterReading.currentL2) + "</div>\n";
  ptr += "<div class=\"card card-off\">" + String(meterReading.currentL3) + "</div>\n";

  ptr += "</body>\n";
  ptr += "</html>\n";

  return ptr;
}