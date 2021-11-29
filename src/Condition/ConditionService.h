#ifndef __CONDITION_SERVICE_H__
#define __CONDITION_SERVICE_H__

#include "Condition.h"
#include <WString.h>
#include "SPIFFS.h"

class ConditionService {
public:

  bool storeConditionDevices();
  void loadConditionDevices();
  std::unordered_map<uint8_t, ConditionDevice>& getConditionDevices();

  void storeConditionDevice(ConditionDevice& device);
  void updateConditionDevice(const ConditionDevice& device);
  void deleteConditionDevice(uint8_t deviceId);

private:
  std::unordered_map<uint8_t, ConditionDevice> _devices;

  String _devicesFile = "/devices.csv";
  String _actionsFile = "/actions.csv";
  String _rulesFile = "/rules.csv";

  // for storing
  String conditionDeviceStructToCsv(ConditionDevice& device) const;
  String conditionDeviceActionStructToCsv(ConditionDeviceAction& action) const;
  String conditionDeviceActionRuleStructToCsv(ConditionDeviceActionRule& csvString) const;

  // for loading
  ConditionDevice conditionDeviceCsvToStruct(String& csvString) const;
  ConditionDeviceAction conditionDeviceActionCsvToStruct(String& csvString) const;
  ConditionDeviceActionRule conditionDeviceActionRuleCsvToStruct(String& csvString) const;

  String* explodeCsvLine(String& line) const;
};

#endif
