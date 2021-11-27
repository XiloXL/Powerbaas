#ifndef __CONDITION_H__
#define __CONDITION_H__

#include <stdint.h>
#include <vector>
#include <unordered_map>

#define DEVICE_ENABLED 1                // Device is enabled
#define DEVICE_DISABLED 2               // Device is disabled
#define DEVICE_ON 1                     // Device currently on
#define DEVICE_OFF 2                    // Device currently off
#define DEVICE_SOLAR 1                  // Device of type solar
#define DEVICE_SWITCH 2                 // Device of type switch
#define DEVICE_GRID 3                   // Device of type grid for prices

#define SOLAR_DEVICE_GROWATT 1          // Device of type solar and class growatt
#define SOLAR_DEVICE_MYENERGI 2         // Device of type solar and class myenergi
#define SOLAR_DEVICE_ZONNEPLAN 3        // Device of type solar and class zonneplan

#define SWITCH_DEVICE_433 1             // Device of type switch and class 433
#define SWITCH_DEVICE_SONOFF 2          // Device of type switch and class sonoff

#define GRID_DEVICE_DUMMY 1             // Grid prices dummy for testing
#define GRID_DEVICE_NUON 2              // Grid prices Nuon

#define ACTION_TYPE_ON 1                // Turn device on
#define ACTION_TYPE_OFF 2               // Turn device off
#define ACTION_TYPE_RECEIVE 3           // Perform request to external server

#define RULE_TYPE_TIME 1                // Specific time in GMT
#define RULE_TYPE_INTERVAL 2            // Every x amount of time
#define RULE_TYPE_OVERSUPPLY 3          // Powerusage must be negative
#define RULE_TYPE_SHORTAGE 4            // Powerusage must be positive
#define RULE_TYPE_TEMPERATURE 5         // Temperature must be met
#define RULE_TYPE_GRIDPRICE 6           // Grid price must be met

#define CONDITION_EQUALS 1              // Condition value has to be
#define CONDITION_GREATER_THAN 2        // Condition value must be greater than
#define CONDITION_LESS_THAN 3           // Condition value mest be less than

struct ConditionDeviceActionRule {
  uint32_t deviceId;                    // => Foreign key to device
  uint8_t actionId;                     // => Foreign key to action
  uint8_t type;                         // => Oversupply
  uint8_t condition;                    // => Greater than
  uint32_t value;                       // => Watt/milliseconds/etc
  uint32_t threshold;                   // => Amount of time condition must be met in millis()
  uint32_t thresholdOffset;             // => Time condition first met in millis()
  uint8_t targetDeviceId;               // => Specific device, bv TemperatureDevice living room
};

struct ConditionDeviceAction {
  uint8_t id;                           // => Unique increment
  uint32_t deviceId;                    // => Foreign key to device
  uint8_t type;                         // => On/Off/Request
  uint32_t activeFrom;                  // => Active from in GMT
  uint32_t activeUntil;                 // => Active until in GMT
  std::vector<ConditionDeviceActionRule> rules;
};

struct ConditionDevice {
  uint32_t id = 0;                      // => Unique increment
  uint8_t state = DEVICE_OFF;           // => On or Off
  uint8_t type = 0;                     // => Solar/Switch device
  uint8_t device = 0;                   // => Growatt/Zonneplan device
  uint8_t priority = 0;                 // => Highest priority
  uint8_t enabled = 0;                  // => Can be turned on/off
  uint32_t latestExecution = 0;         // => Last moment turned on/off/executed in millis()
  char name[64];                        // => Name to display
  uint32_t overruledUntil = 0;          // => Overrule conditionMachine until in millis()
  std::unordered_map<uint8_t, ConditionDeviceAction> actions;
};

#endif
