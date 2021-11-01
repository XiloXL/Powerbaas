#ifndef __METER_READING_H__
#define __METER_READING_H__

#include <stdint.h>

struct MeterReading {
  // this is the amount of power you are currently using, negative if you return to the net, positive if you use from the net
  int32_t powerUsage;

  // the amount of power you are using from the net, low and high rates (some smart meters use 2 rates)
  uint32_t powerDeliverHigh;
  uint32_t powerDeliverLow;

  // the amount of power you are returning to the net, low and high rates (some smart meters use 2 rates)
  uint32_t powerReturnHigh;
  uint32_t powerReturnLow;

  // the amount of gas you are using from the net
  uint32_t gas;
};

#endif
