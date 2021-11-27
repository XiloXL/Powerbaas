#ifndef __CONDITION_RULE_H__
#define __CONDITION_RULE_H__

#include "../Condition.h"

class ConditionRule {
public:
  virtual ~ConditionRule() {}
  virtual bool run(ConditionDevice& device, ConditionDeviceActionRule& rule, uint32_t currentMillis) = 0;
};

#endif


