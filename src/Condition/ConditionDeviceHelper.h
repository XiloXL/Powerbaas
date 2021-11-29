#ifndef __CONDITION_DEVICE_HELPER_H__
#define __CONDITION_DEVICE_HELPER_H__

#include "Condition.h";

struct OnOffConditions {
  uint8_t startHour = 0;
  uint8_t stopHour = 0;
  uint16_t oversupplyValue = 0;
  uint16_t shortageValue = 0;
  uint32_t secondsOfShortage = 0;
  uint32_t secondsOfOversupply = 0;
};

class ConditionDeviceHelper {
public:

  static void storeOnOffConditionsToDevice(ConditionDevice& device, OnOffConditions& conditions) {

    // clear actions
    device.actions.clear();

    // on action
    ConditionDeviceAction actionOn;
    actionOn.id = 1;
    actionOn.deviceId = device.id;
    actionOn.type = ACTION_TYPE_ON;

    // start if its later than startHour
    if(conditions.startHour > 0) {
      ConditionDeviceActionRule ruleOnStart;
      ruleOnStart.deviceId = device.id;
      ruleOnStart.actionId = actionOn.id;
      ruleOnStart.type = RULE_TYPE_TIME;
      ruleOnStart.condition = CONDITION_GREATER_THAN;
      ruleOnStart.value = (conditions.startHour * 3600);
      ruleOnStart.threshold = 0;
      actionOn.rules.push_back(ruleOnStart);
    }
    // and earlier than stopHour
    if(conditions.stopHour > 0) {
      ConditionDeviceActionRule ruleOnStop;
      ruleOnStop.deviceId = device.id;
      ruleOnStop.actionId = actionOn.id;
      ruleOnStop.type = RULE_TYPE_TIME;
      ruleOnStop.condition = CONDITION_LESS_THAN;
      ruleOnStop.value = (conditions.stopHour * 3600);
      ruleOnStop.threshold = 0;
      actionOn.rules.push_back(ruleOnStop);
    }
    // and oversupply greater than oversupplyValue for given time
    if(conditions.oversupplyValue > 0) {
      ConditionDeviceActionRule ruleOnOversupply;
      ruleOnOversupply.deviceId = device.id;
      ruleOnOversupply.actionId = actionOn.id;
      ruleOnOversupply.type = RULE_TYPE_OVERSUPPLY;
      ruleOnOversupply.condition = CONDITION_GREATER_THAN;
      ruleOnOversupply.value = conditions.oversupplyValue;
      ruleOnOversupply.threshold = (conditions.secondsOfOversupply * 1000);
      actionOn.rules.push_back(ruleOnOversupply);
    }

    device.actions[actionOn.id] = actionOn;

    // stop before startHour
    if(conditions.startHour > 0) {
      ConditionDeviceAction actionOff;
      actionOff.id = 2;
      actionOff.deviceId = device.id;
      actionOff.type = ACTION_TYPE_OFF;

      ConditionDeviceActionRule ruleOff;
      ruleOff.deviceId = device.id;
      ruleOff.actionId = actionOff.id;
      ruleOff.type = RULE_TYPE_TIME;
      ruleOff.condition = CONDITION_LESS_THAN;
      ruleOff.value = (conditions.startHour * 3600);
      ruleOff.threshold = 0;
      actionOff.rules.push_back(ruleOff);

      device.actions[actionOff.id] = actionOff;
    }

    // stop after stopHour
    if(conditions.stopHour > 0) {
      ConditionDeviceAction actionOff;
      actionOff.id = 3;
      actionOff.deviceId = device.id;
      actionOff.type = ACTION_TYPE_OFF;

      ConditionDeviceActionRule ruleOff;
      ruleOff.deviceId = device.id;
      ruleOff.actionId = actionOff.id;
      ruleOff.type = RULE_TYPE_TIME;
      ruleOff.condition = CONDITION_GREATER_THAN;
      ruleOff.value = (conditions.stopHour * 3600);
      ruleOff.threshold = 0;
      actionOff.rules.push_back(ruleOff);

      device.actions[actionOff.id] = actionOff;
    }

    // stop on shortage
    if(conditions.shortageValue > 0) {
      ConditionDeviceAction actionOff;
      actionOff.id = 4;
      actionOff.deviceId = device.id;
      actionOff.type = ACTION_TYPE_OFF;

      ConditionDeviceActionRule ruleOff;
      ruleOff.deviceId = device.id;
      ruleOff.actionId = actionOff.id;
      ruleOff.type = RULE_TYPE_SHORTAGE;
      ruleOff.condition = CONDITION_GREATER_THAN;
      ruleOff.value = conditions.shortageValue;
      ruleOff.threshold = (conditions.secondsOfShortage * 1000);
      actionOff.rules.push_back(ruleOff);

      device.actions[actionOff.id] = actionOff;
    }
  }

  static OnOffConditions getOnOffConditionsFromDevice(ConditionDevice& device) {

    OnOffConditions onoff;

    // start hour
    ConditionDeviceActionRule rule = getRule(device, ACTION_TYPE_ON, RULE_TYPE_TIME, CONDITION_GREATER_THAN);
    if(rule.value > 0 ) {
      onoff.startHour = rule.value / 3600;
    }

    // stop hour
    rule = getRule(device, ACTION_TYPE_OFF, RULE_TYPE_TIME, CONDITION_GREATER_THAN);
    if(rule.value > 0 ) {
      onoff.stopHour = rule.value / 3600;
    }

    // oversupply value
    rule = getRule(device, ACTION_TYPE_ON, RULE_TYPE_OVERSUPPLY, CONDITION_GREATER_THAN);
    if(rule.value > 0) {
      onoff.oversupplyValue = rule.value;
      onoff.secondsOfOversupply = rule.threshold / 1000;
    }

    // shortage value
    rule = getRule(device, ACTION_TYPE_OFF, RULE_TYPE_SHORTAGE, CONDITION_GREATER_THAN);
    if(rule.value > 0) {
      onoff.shortageValue = rule.value;
      onoff.secondsOfShortage = rule.threshold / 1000;
    }

    return onoff;
  }

private:
  static ConditionDeviceActionRule getRule(ConditionDevice& device, uint8_t actionType, uint8_t ruleType, uint8_t condition) {
    for (auto& actionElement: device.actions)  {
      ConditionDeviceAction& action = actionElement.second;
      if(action.type == actionType) {
        for(ConditionDeviceActionRule& rule : action.rules) {
          if(rule.type == ruleType) {
            if(condition == 0) {
              return rule;
            }
            if(condition == rule.condition) {
              return rule;
            }
          }
        }
      }
    }
    ConditionDeviceActionRule emptyRule;
    return emptyRule;
  }
};

#endif