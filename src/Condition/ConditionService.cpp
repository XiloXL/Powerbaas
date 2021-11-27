#include "ConditionService.h"
#include "FS.h"
#include "SPIFFS.h"

bool ConditionService::storeConditionDevices() {

  String devicesCsv = "";
  String actionsCsv = "";
  String rulesCsv = "";

  // foreach device
  for (auto& deviceElement: _devices) {
    ConditionDevice& device = deviceElement.second;
    devicesCsv = "\n" + conditionDeviceStructToCsv(device) + devicesCsv;

    // foreach action
    for (auto& actionElement: device.actions)  {
      ConditionDeviceAction& action = actionElement.second;
      actionsCsv = "\n" + conditionDeviceActionStructToCsv(action) + actionsCsv;

      // foreach rule
      for(ConditionDeviceActionRule& rule : action.rules) {
        rulesCsv = "\n" + conditionDeviceActionRuleStructToCsv(rule) + rulesCsv;
      }
    }
  }

  // remove old files
  SPIFFS.remove(_devicesFile.c_str());
  SPIFFS.remove(_actionsFile.c_str());
  SPIFFS.remove(_rulesFile.c_str());

  // create devices csv
  File file = SPIFFS.open(_devicesFile.c_str(), FILE_WRITE);
  file.print(devicesCsv.substring(1).c_str());
  file.close();

  // create actions csv
  file = SPIFFS.open(_actionsFile.c_str(), FILE_WRITE);
  file.print(actionsCsv.substring(1).c_str());
  file.close();

  // create rules csv
  file = SPIFFS.open(_rulesFile.c_str(), FILE_WRITE);
  file.print(rulesCsv.substring(1).c_str());
  file.close();

  return true;
}

void ConditionService::loadConditionDevices() {

  // first clear
  _devices.clear();

  // DEVICES
  String content = "";
  File file = SPIFFS.open(_devicesFile.c_str());
  if (file) {
    while (file.available()) {
      char ltr = file.read();
      if(ltr == '\n'){
        ConditionDevice device = conditionDeviceCsvToStruct(content);
        _devices[device.id] = device;
        content = "";
      } else {
        content += ltr;
      }
    }
    ConditionDevice device = conditionDeviceCsvToStruct(content);
    _devices[device.id] = device;
    file.close();
    content = "";
  }

  // ACTIONS
  file = SPIFFS.open(_actionsFile.c_str());
  if (file) {
    while (file.available()) {
      char ltr = file.read();
      if(ltr == '\n'){
        ConditionDeviceAction action = conditionDeviceActionCsvToStruct(content);
        if(_devices.count(action.deviceId) == 1) {
          _devices[action.deviceId].actions[action.id] = action;
        }
        content = "";
      } else {
        content += ltr;
      }
    }
    ConditionDeviceAction action = conditionDeviceActionCsvToStruct(content);
     if(_devices.count(action.deviceId) == 1) {
      _devices[action.deviceId].actions[action.id] = action;
    }
    file.close();
    content = "";
  }

  // RULES
  file = SPIFFS.open(_rulesFile.c_str());
  if (file) {
    while (file.available()) {
      char ltr = file.read();
      if(ltr == '\n'){
        ConditionDeviceActionRule rule = conditionDeviceActionRuleCsvToStruct(content);
        if(_devices.count(rule.deviceId) == 1) {
          _devices[rule.deviceId].actions[rule.actionId].rules.push_back(rule);
        }
        content = "";
      } else {
        content += ltr;
      }
    }

    ConditionDeviceActionRule rule = conditionDeviceActionRuleCsvToStruct(content);
    if(_devices.count(rule.deviceId) == 1) {
      _devices[rule.deviceId].actions[rule.actionId].rules.push_back(rule);
    }
    file.close();
    content = "";
  }
}



std::unordered_map<uint8_t, ConditionDevice>& ConditionService::getConditionDevices() {

  // is al gevuld, dus geef gelijk terug!
  if(_devices.size() == 0) {
    loadConditionDevices();
  }

  return _devices;
}

void ConditionService::storeConditionDevice(ConditionDevice& device) {

  // complete new device
  if(device.id == 0) {
    device.id = esp_random() % 10000000;
  }

  // add device to map
  _devices[device.id] = device;

  // store map
  storeConditionDevices();
}

void ConditionService::updateConditionDevice(const ConditionDevice& device) {

  // guard device.id is > 0
  if(device.id == 0) {
    return;
  }

  // guard device to be updated exists
  if(_devices.count(device.id) == 0) {
    return;
  }

  // update device in map
  _devices[device.id] = device;

  // store map
  storeConditionDevices();
}

void ConditionService::deleteConditionDevice(uint8_t deviceId) {

  // guard device to be updated exists
  if(_devices.count(deviceId) == 0) {
    return;
  }

  // remove device
  _devices.erase(deviceId);

  // store map
  storeConditionDevices();
}



ConditionDevice ConditionService::conditionDeviceCsvToStruct(String& csvString) const {

  String* exploded = explodeCsvLine(csvString);

  ConditionDevice device;

  device.id                = strtoul((char*)exploded[0].c_str(), NULL, 0);
  device.state             = strtoul((char*)exploded[1].c_str(), NULL, 0);
  device.type              = strtoul((char*)exploded[2].c_str(), NULL, 0);
  device.device            = strtoul((char*)exploded[3].c_str(), NULL, 0);
  device.priority          = strtoul((char*)exploded[4].c_str(), NULL, 0);
  device.enabled           = strtoul((char*)exploded[5].c_str(), NULL, 0);
  device.latestExecution   = strtoul((char*)exploded[6].c_str(), NULL, 0);
  strcpy(device.name, exploded[7].c_str());
  device.overruledUntil    = 0;

  return device;
}

ConditionDeviceAction ConditionService::conditionDeviceActionCsvToStruct(String& csvString) const {

  String* exploded = explodeCsvLine(csvString);

  ConditionDeviceAction action;
  action.id         = strtoul((char*)exploded[0].c_str(), NULL, 0);
  action.deviceId  = strtoul((char*)exploded[1].c_str(), NULL, 0);
  action.type       = strtoul((char*)exploded[2].c_str(), NULL, 0);

  return action;
}

ConditionDeviceActionRule ConditionService::conditionDeviceActionRuleCsvToStruct(String& csvString) const {

  String* exploded = explodeCsvLine(csvString);

  ConditionDeviceActionRule rule;
  rule.deviceId        = strtoul((char*)exploded[0].c_str(), NULL, 0);
  rule.actionId        = strtoul((char*)exploded[1].c_str(), NULL, 0);
  rule.type             = strtoul((char*)exploded[2].c_str(), NULL, 0);
  rule.condition        = strtoul((char*)exploded[3].c_str(), NULL, 0);
  rule.value            = strtoul((char*)exploded[4].c_str(), NULL, 0);
  rule.threshold        = strtoul((char*)exploded[5].c_str(), NULL, 0);
  rule.thresholdOffset = 0;
  rule.targetDeviceId = strtoul((char*)exploded[6].c_str(), NULL, 0);

  return rule;
}

String* ConditionService::explodeCsvLine(String& line) const {

  static String ding2[20];

  int index = 0;
  const char s[2] = ",";
  char str[256] = "";

  line.toCharArray(str, 256);

  char* token;
  token = strtok(str, s);
  while(token != NULL) {
    ding2[index] = token;
    index++;
    token = strtok(NULL, s);
  }

  return ding2;
}

String ConditionService::conditionDeviceStructToCsv(ConditionDevice& device) const {
  String csv = "";
  csv += String(device.id) + ",";
  csv += String(device.state) + ",";
  csv += String(device.type) + ",";
  csv += String(device.device) + ",";
  csv += String(device.priority) + ",";
  csv += String(device.enabled) + ",";
  csv += String(0) + ","; // force latestExecution 0, or else problems after rebooting
  csv += String(device.name);
  return csv;
}

String ConditionService::conditionDeviceActionStructToCsv(ConditionDeviceAction& action) const {
  String csv = "";
  csv += String(action.id) + ",";
  csv += String(action.deviceId) + ",";
  csv += String(action.type);
  return csv;
}

String ConditionService::conditionDeviceActionRuleStructToCsv(ConditionDeviceActionRule& rule) const {
  String csv = "";
  csv += String(rule.deviceId) + ",";
  csv += String(rule.actionId) + ",";
  csv += String(rule.type) + ",";
  csv += String(rule.condition) + ",";
  csv += String(rule.value) + ",";
  csv += String(rule.threshold) + ",";
  csv += String(rule.targetDeviceId);
  return csv;
}
