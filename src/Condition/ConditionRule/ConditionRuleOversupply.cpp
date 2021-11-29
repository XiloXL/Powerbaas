#include "ConditionRuleOversupply.h"

bool ConditionRuleOversupply::run(ConditionDevice& device, ConditionDeviceActionRule& rule, uint32_t currentMillis) {

  if(-_meterReading.powerUsage < (int32_t)rule.value) {
    rule.thresholdOffset = currentMillis;
    return false;
  }

  if(rule.threshold == 0) {
    return true;
  }

  if(rule.thresholdOffset == 0) {
    rule.thresholdOffset = currentMillis;
    return false;
  }

  if((currentMillis - rule.thresholdOffset) > rule.threshold) {
     rule.thresholdOffset = currentMillis;
     return true;
  }

  return false;
}
