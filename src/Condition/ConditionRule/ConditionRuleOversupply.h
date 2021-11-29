#ifndef __CONDITION_RULE_OVERSUPPLY_H__
#define __CONDITION_RULE_OVERSUPPLY_H__

#include "ConditionRule.h"
#include "../../MeterReading/MeterReading.h"

class ConditionRuleOversupply : public ConditionRule {
public:
  ConditionRuleOversupply(MeterReading& meterReading) : _meterReading(meterReading) {}

  virtual bool run(ConditionDevice& device, ConditionDeviceActionRule& rule, uint32_t currentMillis);

private:
  MeterReading& _meterReading;
};

#endif
