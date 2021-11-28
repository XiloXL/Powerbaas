#include <Powerbaas.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <uri/UriBraces.h>

const char* ssid = "YOUR-SSID";
const char* password = "YOUR-PASSWORD";

Powerbaas powerbaas(true);
MeterReading meterReading;
SystemTime systemTime;
WebServer server(80);
Timer timer1;
ConditionService conditionService;
ConditionMachine conditionMachine(conditionService, meterReading, systemTime);

void setup() {
  setupSerial();
  setupSpiffs();
  setupPowerbaas();
  setupWebserver();
  setupSystemTime();
  setupEndpoints();
  setupConditionMachine();
}

void setupSerial() {
  Serial.begin(115200);
  delay(3000);
}

void setupSpiffs() {
  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
  }
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
}

void setupSystemTime() {
  systemTime.syncWithNTP();

  //loop 1 every hour, start 2 seconds later
  timer1.runEvery(3600000, 2000, [](){
    systemTime.syncWithNTP();
  });
}

void setupConditionMachine() {
  // loop 1 every second
  timer1.runEvery(1000, 0, []() {
    conditionMachine.run();
  });
}


void setupEndpoints() {

  // index
  server.on("/", []() {
    server.send(200, "text/html", index());
  });
  // add switch form
  server.on("/switch/add", []() {
    server.send(200, "text/html", addSwitch());
  });
  // edit switch form
  server.on(UriBraces("/switch/edit/{}"), []() {
    String switchId = server.pathArg(0);
    std::unordered_map<uint8_t, ConditionDevice>& devices = conditionService.getConditionDevices();
    ConditionDevice& device = devices[switchId.toInt()];
    server.send(200, "text/html", editSwitch(device));
  });
  // edit switch form
  server.on(UriBraces("/switch/delete/{}"), []() {
    String switchId = server.pathArg(0);
    std::unordered_map<uint8_t, ConditionDevice>& devices = conditionService.getConditionDevices();
    ConditionDevice& device = devices[switchId.toInt()];
    server.send(200, "text/html", deleteSwitch(device));
  });
  // pair mode form
  server.on(UriBraces("/switch/pair/{}"), []() {
    String switchId = server.pathArg(0);
    std::unordered_map<uint8_t, ConditionDevice>& devices = conditionService.getConditionDevices();
    ConditionDevice& device = devices[switchId.toInt()];
    server.send(200, "text/html", pairSwitch(device));
  });
  // pair mode form
  server.on(UriBraces("/handle/switch/pair/{}"), []() {
    String switchId = server.pathArg(0);
    std::unordered_map<uint8_t, ConditionDevice>& devices = conditionService.getConditionDevices();
    ConditionDevice& device = devices[switchId.toInt()];
    server.send(200, "text/html", pairSwitchHandler(device));
    handleSwitchPair(device);
  });


  // handle add switch form
  server.on(UriBraces("/handle/switch/add"), []() {
    handleSwitchAdd(server.arg("name"));
    server.send(200, "text/html", redirect());
  });
  // handle edit switch form
  server.on(UriBraces("/handle/switch/edit"), []() {
    String switchId = server.arg("id");
    String name = server.arg("name");
    std::unordered_map<uint8_t, ConditionDevice>& devices = conditionService.getConditionDevices();
    ConditionDevice& device = devices[switchId.toInt()];
    handleSwitchEdit(device, name);
    server.send(200, "text/html", redirect());
  });
  // handle delete switch form
  server.on(UriBraces("/handle/switch/delete/{}"), []() {
    String switchId = server.pathArg(0);
    std::unordered_map<uint8_t, ConditionDevice>& devices = conditionService.getConditionDevices();
    ConditionDevice& device = devices[switchId.toInt()];
    handleSwitchDelete(device);
    server.send(200, "text/html", redirect());
  });
  // turn switch on
  server.on(UriBraces("/handle/switch/on/{}"), []() {
    String switchId = server.pathArg(0);
    std::unordered_map<uint8_t, ConditionDevice>& devices = conditionService.getConditionDevices();
    ConditionDevice& device = devices[switchId.toInt()];
    handleSwitchOn(device);
    server.send(200, "text/html", redirect());
  });
  // turn switch off
  server.on(UriBraces("/handle/switch/off/{}"), []() {
    String switchId = server.pathArg(0);
    std::unordered_map<uint8_t, ConditionDevice>& devices = conditionService.getConditionDevices();
    ConditionDevice& device = devices[switchId.toInt()];
    handleSwitchOff(device);
    server.send(200, "text/html", redirect());
  });
  // update switch conditions
  server.on(UriBraces("/handle/switch/conditions/{}"), []() {
    String switchId = server.pathArg(0);
    std::unordered_map<uint8_t, ConditionDevice>& devices = conditionService.getConditionDevices();
    ConditionDevice& device = devices[switchId.toInt()];
    String onTime = server.arg("on_time");
    String onSurplus = server.arg("on_surplus");
    String onDuration = server.arg("on_duration");
    String offTime = server.arg("off_time");
    String offShortage = server.arg("off_shortage");
    String offDuration = server.arg("off_duration");
    handleSwitchConditions(device, onTime, onSurplus, onDuration, offTime, offShortage, offDuration);
    server.send(200, "text/html", redirect());
  });


  server.begin();
}

void loop() {
  timer1.update();
  powerbaas.receive();
  server.handleClient();
}



/***************************************
 *** Process actions from HTML pages ***
 ***************************************/

void handleSwitchAdd(String name) {
  ConditionDevice device;
  device.id = esp_random() % 10000000;
  strcpy(device.name, name.c_str());
  device.state = DEVICE_OFF;
  device.type = DEVICE_SWITCH;
  device.enabled = DEVICE_ENABLED;
  std::unordered_map<uint8_t, ConditionDevice>& devices = conditionService.getConditionDevices();
  devices[device.id] = device;
  conditionService.storeConditionDevices();
}

void handleSwitchEdit(ConditionDevice& device, String name) {
  strcpy(device.name, name.c_str());
  conditionService.storeConditionDevices();
}

void handleSwitchDelete(ConditionDevice& device) {
  std::unordered_map<uint8_t, ConditionDevice>& devices = conditionService.getConditionDevices();
  devices.erase(device.id);
  conditionService.storeConditionDevices();
}

void handleSwitchOn(ConditionDevice& device) {
  device.state = DEVICE_ON;
  NewRemoteTransmitter transmitter(device.id, 12, 232, 3);
  transmitter.sendUnit(0, true);
  // todo: overrule
}

void handleSwitchOff(ConditionDevice& device) {
  device.state = DEVICE_OFF;
  NewRemoteTransmitter transmitter(device.id, 12, 232, 3);
  transmitter.sendUnit(0, false);
  // todo: overrule
}
void handleSwitchPair(ConditionDevice& device) {
  NewRemoteTransmitter transmitter(device.id, 12, 232, 3);
  for(int i = 0; i < 9; i++) {
    transmitter.sendUnit(0, true);
    delay(900);
  }
}
void handleSwitchConditions(ConditionDevice& device, String onTime, String onSurplus, String onDuration, String offTime, String offShortage, String offDuration) {
  Serial.println(onTime);
  Serial.println(onSurplus);
  Serial.println(onDuration);
  Serial.println(offTime);
  Serial.println(offShortage);
  Serial.println(offDuration);
}



/***************************************
 *** HTML pages and forms **************
 ***************************************/
String index() {

  String body = "<h3>Current power usage</h3>\n";
  body += "<div class=\"card card-on\">" + String(meterReading.powerUsage) + " Watt</div>\n";

  // Switch
  body += "<h3>Smart Switches</h3>\n";
  std::unordered_map<uint8_t, ConditionDevice>& devices = conditionService.getConditionDevices();
  for (auto& deviceElement: devices) {
    ConditionDevice& device = deviceElement.second;
    if(device.state == DEVICE_ON) {
      body += "<a href=\"/switch/edit/" + String(device.id) + "\" class=\"card card-on\">" + String(device.name) + "</a>\n";
    }
    else if(device.state == DEVICE_OFF) {
      body += "<a href=\"/switch/edit/" + String(device.id) + "\" class=\"card card-off\">" + String(device.name) + "</a>\n";
    }
  }
  body += "<a href=\"/switch/add\" class=\"card card-grey\">" + String("+") + "</a>\n";

  return html(body);
}

String addSwitch() {

  String body = "<h3>Add Smart Switch</h3>\n";
  body += "<form action=\"/handle/switch/add\" method=\"GET\">\n";
  body += "<div>Name:</div><div><input type=\"text\" name=\"name\"</div>\n";
  body += "<div><input type=\"submit\" value=\"Save\" /></div>\n";
  body += "</form>\n";

  return html(body);
}

String editSwitch(ConditionDevice& device) {

  // edit name
  String body = "<h3>Edit Smart Switch " + String(device.name) + " (" + String(device.id) + ") </h3>\n";
  body += "<form action=\"/handle/switch/edit\" method=\"GET\">\n";
  body += "<input type=\"hidden\" name=\"id\" value=\"" + String(device.id) + "\">";
  body += "<div>Name:</div><div><input type=\"text\" value=\"" + String(device.name) + "\" name=\"name\"></div>\n";
  body += "<div><input type=\"submit\" /></div>\n";
  body += "</form>";

  // turn of/of, pair, delete
  body += "<h3>Actions</h3>";
  if(device.state == DEVICE_ON) {
    body += "<a href=\"/handle/switch/off/" + String(device.id) + "\" class=\"card card-on\">Off</a>\n";
  }
  else if(device.state == DEVICE_OFF) {
    body += "<a href=\"/handle/switch/on/" + String(device.id) + "\" class=\"card card-off\">On</a>\n";
  }
  body += "<a href=\"/switch/pair/" + String(device.id) + "\" class=\"card card-action\">Pair</a>\n";
  body += "<a href=\"/switch/delete/" + String(device.id) + "\" class=\"card card-red\">Delete</a>\n";

  // turn device on at time
  body += "<form action=\"/handle/switch/conditions/" + String(device.id) + "\" method=\"GET\">\n";
  body += "<hr /><h3>Turn device on</h3>\n";

  body += "<table><tr>\n";
  body += "<td>Turn device on at time:</td></tr><tr>\n";
  body += "<td><input name=\"on_time\" type=\"range\" value=\"0\" min=\"0\" max=\"24\" oninput=\"document.getElementById('on_time').value = this.value\"></td>\n";
  body += "<td><output id=\"on_time\">0</output> hour</td></tr>\n";

  // when there is solar surplus of:
  body += "<tr><td>When there is solar surplus of:</td></tr><tr>\n";
  body += "<td><input name=\"on_surplus\" type=\"range\" value=\"0\" min=\"0\" max=\"3000\" oninput=\"document.getElementById('on_surplus').value = this.value\"></td>\n";
  body += "<td><output id=\"on_surplus\">0</output> watts</td></tr>\n";

  // for a duration of:
  body += "<tr><td>For a duration of:</td></tr><tr>\n";
  body += "<td><input name=\"on_duration\" type=\"range\" value=\"0\" min=\"0\" max=\"300\" oninput=\"document.getElementById('on_duration').value = this.value\"></td>\n";
  body += "<td><output id=\"on_duration\">0</output> sec</td></tr></table>\n";


  // turn device off conditions
  body += "<hr /><h3>Turn device off</h3>\n";
  body += "<table><tr>\n";
  body += "<td>Turn device off at time:</td></tr><tr>\n";
  body += "<td><input name=\"off_time\" type=\"range\" value=\"0\" min=\"0\" max=\"24\" oninput=\"document.getElementById('off_time').value = this.value\"></td>\n";
  body += "<td><output id=\"off_time\">0</output> hour</td></tr>\n";

  // when there is power shortage  of:
  body += "<tr><td>When there is power shortage of:</td></tr><tr>\n";
  body += "<td><input name=\"off_shortage\" type=\"range\" value=\"0\" min=\"0\" max=\"3000\" oninput=\"document.getElementById('off_shortage').value = this.value\"></td>\n";
  body += "<td><output id=\"off_shortage\">0</output> watts</td></tr>\n";

  // for a duration of:
  body += "<tr><td>For a duration of:</td></tr><tr>\n";
  body += "<td><input name=\"off_duration\" type=\"range\" value=\"0\" min=\"0\" max=\"300\" oninput=\"document.getElementById('off_duration').value = this.value\"></td>\n";
  body += "<td><output id=\"off_duration\">0</output> sec</td></tr></table><hr />\n";
  body += "<input type=\"submit\" /></div>\n";
  body += "<br /><br /><br /><br /><br />\n";
  body += "</div>\n";
  body += "</form>\n";

  return html(body);
}

String deleteSwitch(ConditionDevice& device) {
  String body = "<h3>Delete Smart Switch " + String(device.name) + " </h3>\n";
  body += "<div>Are you sure you want to delete this switch?<br /><br /></div>\n";
  body += "<a href=\"/handle/switch/delete/" + String(device.id) + "\" class=\"card card-red\">Yes!</a>\n";
  body += "<a href=\"/\" class=\"card card-grey\">Cancel</a>\n";

  return html(body);
}

String pairSwitch(ConditionDevice& device) {
  String body = "<h3>Pair Smart Switch " + String(device.name) + " </h3>\n";
  body += "<div>First press <b>Start Pairing</b> and then put switch in socket.<br /><br />The pairing procedure will take about 10 seconds.<br /><br /></div>\n";
  body += "<a href=\"/handle/switch/pair/" + String(device.id) + "\" class=\"card card-action\">Start Pairing</a>\n";

  return html(body);
}

String pairSwitchHandler(ConditionDevice& device) {
  String body = "<h3>Smart Switch " + String(device.name) + " is now pairing</h3>\n";
  body += "<div class=\"card card-action\" id=\"countdown\">10</div>\n";
  body += R"END(
  <script language="javascript">
    var timeleft = 10;
    var downloadTimer = setInterval(function(){
      if(timeleft <= 0){
        clearInterval(downloadTimer);
        document.getElementById("countdown").innerHTML = "Finished";
        window.location = "/";
      } else {
        document.getElementById("countdown").innerHTML = timeleft + "";
      }
      timeleft -= 1;
    }, 1000);
  </script>
  )END";
  return html(body);
}

String redirect() {
  String ptr = R"END(
  <!DOCTYPE html>
  <html>
  <head>
  <script language=javascript>
  function redirect(){
    window.location = "/";
  }
  </script>
  </head>
  <body onload="redirect()">
  </body>
  </html>
  )END";
  return ptr;
}

String html(String body) {
  String ptr = R"END(
  <!DOCTYPE html>
  <html>
  <head>
  <meta name="viewport" content="width=device-width, initial-scale=1.0, user-scalable=no">
  <!-- <meta http-equiv="refresh" content="2"> -->
  <title>PowerBaas</title>
  <style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}
    body{margin-top: 20px;} h3 {color: #2d2d2d;margin-bottom: 10px margin-top:50px;}
    .card {display: inline-block;width: 80px; background-color: #3498db;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}
    .card-small{width:40px; display:inline-block;}
    .card-on {background-color: #38b54a;}
    .card-off {background-color: #2d2d2d;}
    .card-action {background-color: #ece50a;}
    .card-grey {background-color: #dddddd;}
    .card-red {background-color: #CC0000; }
    p {font-size: 14px;color: #2d2d2d;margin-bottom: 10px;}
    input { padding:10px; border-radius:10px; border:1px solid #cccccc; }
    input[type=submit] { margin-top:25px; }
    input[type=range] { width:300px; }
    td { text-align:left; }
    table { width:90%; max-width:440px; margin-left: auto;margin-right: auto; }
  </style>
  </head>
  <body>
  <a href="/">
  <img style='border:none; width:80%; margin-bottom:25px;' src="https://t.eu1.jwwb.nl/W1292300/7Z67ppdpu6A8hHiS3s7fsxO1-ys=/658x0/filters:quality(70)/f.eu1.jwwb.nl%2Fpublic%2Fs%2Fq%2Fx%2Ftemp-ubafshmstbomdhzsimxh%2Fum7ocq%2Fimage-1.png" />
  </a>
  )END";

  ptr += body;

  ptr += "</body>\n";
  ptr += "</html>\n";

  return ptr;
}