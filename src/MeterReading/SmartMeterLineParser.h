#ifndef __SMART_METER_LINE_PARSER_H__
#define __SMART_METER_LINE_PARSER_H__

#include <cstdio>
#include <WString.h>
#include "MeterReading.h"

class SmartMeterLineParser {
  public:
    void parse(char* line, MeterReading& meterReading);
    void parseNetherlands(char* line, MeterReading& meterReading);
    void parseBelgium(char* line, MeterReading& meterReading);
    bool isBelgium() {
      return _isBelgium;
    }
    void setBelgium(bool isBelgium) {
      _isBelgium = isBelgium;
    }
  private:
    uint32_t resultToInt(const double& result);
    bool _isBelgium = false;
    uint8_t _currentMbusDevice = 0;
    uint8_t _currentMbusDeviceType = 0;
};

#endif