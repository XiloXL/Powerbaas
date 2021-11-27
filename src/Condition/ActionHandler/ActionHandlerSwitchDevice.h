#ifndef __ACTION_HANDLER_SWITCH_DEVICE_H__
#define __ACTION_HANDLER_SWITCH_DEVICE_H__

#include "ActionHandler.h"
#include "../ConditionRule/ConditionRuleOversupply.h"
#include "../ConditionRule/ConditionRuleShortage.h"
#include "../ConditionRule/ConditionRuleTime.h"

class ActionHandlerSwitchDevice : public ActionHandler {
public:
  ActionHandlerSwitchDevice(ConditionRuleOversupply& conditionRuleOversupply,
    ConditionRuleShortage& conditionRuleShortage,
    ConditionRuleTime& conditionRuleTime
  ):
    _conditionRuleOversupply(conditionRuleOversupply),
    _conditionRuleShortage(conditionRuleShortage),
    _conditionRuleTime(conditionRuleTime)
  {}

  virtual bool handle(ConditionDevice& device, ConditionDeviceAction& action, uint32_t currentMillis);

private:
  ConditionRuleOversupply& _conditionRuleOversupply;
  ConditionRuleShortage& _conditionRuleShortage;
  ConditionRuleTime& _conditionRuleTime;
};

#endif