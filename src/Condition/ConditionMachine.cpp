#include "ConditionMachine.h"

void ConditionMachine::run() {

  _currentMillis = millis();

  for (auto& device_element: _conditionService.getConditionDevices()) {
    ConditionDevice& device = device_element.second;

    // empty device, next!
    if(device.id == 0) {
      continue;
    }

    // disabled, next!
    if(device.enabled != DEVICE_ENABLED) {
      continue;
    }

    // overruled, next!
    if(device.overruledUntil > _currentMillis) {
      continue;
    }

    // loop through actions of this device
    for (auto& actionElement: device.actions)  {
     ConditionDeviceAction& action = actionElement.second;

      switch (device.type) {

        case DEVICE_SWITCH:
          _actionHandlerSwitchDevice.handle(device, action, _currentMillis);
          break;
      }
    }
  }
}
