#include "ConditionRuleTime.h"

bool ConditionRuleTime::run(ConditionDevice& device, ConditionDeviceActionRule& rule, uint32_t currentMillis) {

  time_t rawtime = _systemTime.secondsSinceEpochTimezoned();
  struct tm timeinfoNow;
  timeinfoNow = *localtime(&rawtime);
  struct tm timeinfoRule = timeinfoNow;
  time_t timestampNow = mktime(&timeinfoNow);

  uint32_t seconds = rule.value;
  uint32_t minutes = seconds / 60;
  uint32_t hours = minutes / 60;

  timeinfoRule.tm_hour = int(hours);
  timeinfoRule.tm_min = int(minutes%60) ;
  timeinfoRule.tm_sec = int(seconds%60);
  time_t timestampRule = mktime(&timeinfoRule);

  if(rule.condition == CONDITION_GREATER_THAN) {
    return timestampNow > timestampRule;
  }
  else if(rule.condition == CONDITION_LESS_THAN) {
    return timestampNow < timestampRule;
  }

  return false;
}
