#ifndef __POWERBAAS_H__
#define __POWERBAAS_H__

#include <HardwareSerial.h>
#include "MeterReading.h"
#include "SmartMeterAdapter.h"
#include "SmartMeterLineParser.h"

class Powerbaas {
  public:
    Powerbaas(): _smartMeterAdapter(Serial2, _smartMeterLineParser, _meterReading) {}

    void setup();
    void disableDebug();
    MeterReading getMeterReading();

  private:
    bool _debug = true;
    MeterReading _meterReading{0,0,0,0,0,0};
    SmartMeterLineParser _smartMeterLineParser;
    SmartMeterAdapter _smartMeterAdapter;//(Serial2, _smartMeterLineParser, _meterReading)
};

#endif