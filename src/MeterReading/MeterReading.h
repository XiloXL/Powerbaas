#ifndef __METER_READING_H__
#define __METER_READING_H__

#include <stdint.h>
#include <functional>

struct MeterReading {
  // this is the amount of power currently used, negative if you return to the net, positive if you use from the net
  int32_t powerUsage;

  // the amount of power used from the net, low and high rates (some smart meters use 2 rates)
  uint32_t powerDeliverHigh;
  uint32_t powerDeliverLow;

  // the amount of power returned to the net, low and high rates (some smart meters use 2 rates)
  uint32_t powerReturnHigh;
  uint32_t powerReturnLow;

  // the amount of gas used from the net
  uint32_t gas;

  // voltage per phase
  uint16_t voltageL1;
  uint16_t voltageL2;
  uint16_t voltageL3;

  // current per phase
  uint8_t currentL1;
  uint8_t currentL2;
  uint8_t currentL3;
};

typedef std::function<void(const MeterReading&)> MeterReadingCallback;

#endif
