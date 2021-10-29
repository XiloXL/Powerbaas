#ifndef __SMART_METER_LINE_PARSER_H__
#define __SMART_METER_LINE_PARSER_H__

#include <cstdio>
//#include <WString.h>
#include "MeterReading.h"

class SmartMeterLineParser {
  public:
    void parse(char* line, MeterReading& meterReading);
  private:
    uint32_t resultToInt(float& result);
};

#endif