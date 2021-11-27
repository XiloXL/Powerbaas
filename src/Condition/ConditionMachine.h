#ifndef __CONDITION_MACHINE_H__
#define __CONDITION_MACHINE_H__

#include "../System/SystemTime.h"
#include "../MeterReading/MeterReading.h"
#include "../Condition/ConditionRule/ConditionRuleOversupply.h"
#include "../Condition/ConditionRule/ConditionRuleShortage.h"
#include "../Condition/ConditionRule/ConditionRuleTime.h"
#include "../Condition/ActionHandler/ActionHandlerSwitchDevice.h"

class ConditionMachine {
public:
  ConditionMachine(MeterReading& meterReading, SystemTime& systemTime):
    _meterReading(meterReading),
    _systemTime(systemTime),
    _conditionRuleTime(ConditionRuleTime(systemTime)),
    _conditionRuleOversupply(ConditionRuleOversupply(meterReading)),
    _conditionRuleShortage(ConditionRuleShortage(meterReading)),
    _actionHandlerSwitchDevice(ActionHandlerSwitchDevice(_conditionRuleOversupply, _conditionRuleShortage, _conditionRuleTime))
  {}

  void setup(std::unordered_map<uint8_t, ConditionDevice> devices);
  void run();

private:
  std::unordered_map<uint8_t, ConditionDevice> _devices;
  uint32_t _currentMillis = 0;
  MeterReading& _meterReading;
  SystemTime& _systemTime;

  ConditionRuleTime _conditionRuleTime;
  ConditionRuleOversupply _conditionRuleOversupply;
  ConditionRuleShortage _conditionRuleShortage;
  ActionHandlerSwitchDevice _actionHandlerSwitchDevice;
};

#endif