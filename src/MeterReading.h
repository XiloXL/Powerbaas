#ifndef __METER_READING_H__
#define __METER_READING_H__

#include <stdint.h>

struct MeterReading {
  int32_t powerUsage;
  uint32_t powerDeliverHigh;
  uint32_t powerDeliverLow;
  uint32_t powerReturnHigh;
  uint32_t powerReturnLow;
  uint32_t gas;
};

#endif