#ifndef __CONDITION_RULE_TIME_H__
#define __CONDITION_RULE_TIME_H__

#include "ConditionRule.h"
#include "../../System/SystemTime.h"

class ConditionRuleTime : public ConditionRule {
public:
  ConditionRuleTime(SystemTime& systemTime): _systemTime(systemTime) {}
  virtual bool run(ConditionDevice& device, ConditionDeviceActionRule& rule, uint32_t currentMillis);

private:
  SystemTime& _systemTime;
};

#endif
