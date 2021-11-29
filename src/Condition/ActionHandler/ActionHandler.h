#ifndef __ACTION_HANDLER_H__
#define __ACTION_HANDLER_H__

#include "../Condition.h"

class ActionHandler {
public:
  virtual ~ActionHandler() {}
  virtual bool handle(ConditionDevice& device, ConditionDeviceAction& action, uint32_t currentMillis) = 0;
};

#endif