#include <Powerbaas.h>
#include <WiFi.h>
#include <WiFiManager.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>

Timer timer;

Powerbaas powerbaas(true);
MeterReading meterReading;
WebServer server(80);

// credential
CredentialRepository credentialRepository;
CredentialService credentialService("/", credentialRepository);

// current sensor
CalibrationTable calibrationTable;
CurrentSensor currentSensor(18, 34, credentialService, calibrationTable);
bool currentSensorDetected = false;
GeneratedSolar solarReading{0,0,0};

void setup() {
  Serial.begin(115200);
  delay(5000);
  setupWifi();
  currentSensorDetected = currentSensor.setup();
  if(currentSensorDetected) {
    Serial.println("Current sensor detected!");
  } else {
    Serial.println("No current sensor detected!");
  }
  setupPowerbaas();
  setupWebserver();
  setupTimer();
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
    server.send(200, "text/html", indexHtml());
  });

  // Handle status
  server.on("/status", []() {
    server.send(200, "application/json", statusJson());
  });

  // Handle config
  server.on("/config", []() {
    server.send(200, "text/html", configHtml());
  });

  server.begin();
}

void setupTimer() {

  // every second read
  timer.runEvery(1000, 0, []() {
    if (currentSensorDetected) {
       solarReading.current = (uint32_t)currentSensor.getCurrent();
       solarReading.total = (uint32_t)currentSensor.getSensorTotal();
     }
  });

  // every minute, store current sensor total to spiffs
  timer.runEvery(60000, 0, [](){
    if (currentSensorDetected) {
      currentSensor.storeTotal();
    }
  });
}

void loop() {
  powerbaas.receive();
  server.handleClient();
  timer.update();
}

String indexHtml() {
  String ptr = R"END(
    <!DOCTYPE html> <html>
    <head>
    <meta name="viewport" content="width=device-width, initial-scale=1.0, user-scalable=no">
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
  ptr += "<a href=\"/status\" class=\"card card-on\">Readings</a>\n";
  ptr += "<a href=\"/config\" class=\"card card-on\">Config</a>\n";
  ptr += "</body>\n";
  ptr += "</html>\n";

  return ptr;
}

String statusJson() {
  String json = "{\r\n  \"meterReading\": {";
  json += "\r\n    \"timestamp\": " + String(meterReading.timestamp);
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
  json += ",\r\n  \"solarReading\": {";
  json += "\r\n    \"timestamp\": " + String(meterReading.timestamp);
  json += ",\r\n    \"current\": " + String(solarReading.current);
  json += ",\r\n    \"total\": " + String(solarReading.total);
  json += "\r\n  }\r\n}";

  return json;
}

String configHtml() {
  String ptr = R"END(
  <!DOCTYPE html> <html>
  <head>
  <meta name="viewport" content="width=device-width, initial-scale=1.0, user-scalable=no">
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

  if(server.hasArg("total") && server.hasArg("calibration")) {

    // store total
    CurrentSensorTotal total = currentSensor.getTotal();
    total.total = server.arg("total").toFloat();
    Serial.print("total: ");
    Serial.println(total.total);
    currentSensor.setTotal(total);

    // store config
    CurrentSensorCalibration calib = currentSensor.getCalibration();
    calib.calibrateUser = server.arg("calibration").toFloat();
    Serial.print("calib: ");
    Serial.println(calib.calibrateUser);
    currentSensor.setCalibration(calib);

    ptr += "<h2>Stored posted values!</h2>\n";
  }

  ptr += "<form action=\"/config\" method=\"POST\">\n";
  ptr += "Total: <input type=\"text\" name=\"total\" value=\"" + String(currentSensor.getTotal().total) +"\"></br>\n";
  ptr += "Calibration: <input type=\"text\" name=\"calibration\" value=\""+ String(currentSensor.getCalibration().calibrateUser) + "\"></br>\n";
  ptr += "<input type=\"submit\" value=\"Save\">\n";
  ptr += "</form>";

  ptr += "</body>\n";
  ptr += "</html>\n";

  return ptr;
}