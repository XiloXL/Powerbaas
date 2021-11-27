#include "ConditionMachine.h"

void ConditionMachine::setup(std::unordered_map<uint8_t, ConditionDevice> devices) {
  _devices = devices;
}

void ConditionMachine::run() {

  _currentMillis = millis();

  for (auto& device_element: _devices) {
    ConditionDevice& device = device_element.second;

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
