#include "ActionHandlerSwitchDevice.h"
#include "../../NewRemoteSwitch/NewRemoteTransmitter.h"

bool ActionHandlerSwitchDevice::handle(ConditionDevice& device, ConditionDeviceAction& action, uint32_t currentMillis) {

  for(ConditionDeviceActionRule& rule : action.rules) {
    switch (rule.type) {

      case RULE_TYPE_TIME:
        if(_conditionRuleTime.run(device, rule, currentMillis) == false) {
          return false;
        }
        break;

      case RULE_TYPE_OVERSUPPLY:
        if(_conditionRuleOversupply.run(device, rule, currentMillis) == false) {
          return false;
        }
        break;

      case RULE_TYPE_SHORTAGE:
        if(_conditionRuleShortage.run(device, rule, currentMillis) == false) {
          return false;
        }
        break;
    }
  }

  // turn off
  if(action.type == ACTION_TYPE_OFF) {
    if(device.state == DEVICE_ON) {
      NewRemoteTransmitter transmitter(device.id, 12, 232, 3);
      transmitter.sendUnit(0, false);
      device.state = DEVICE_OFF;
    }
  }
  // turn on
  else if(action.type == ACTION_TYPE_ON) {
    if(device.state == DEVICE_OFF) {
      NewRemoteTransmitter transmitter(device.id, 12, 232, 3);
      transmitter.sendUnit(0, true);
      device.state = DEVICE_ON;
    }
  }

  return true;
}