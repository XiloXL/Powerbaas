#ifndef __POWERBAAS_H__
#define __POWERBAAS_H__

#include <HardwareSerial.h>
#include "MeterReading.h"
#include "SmartMeterAdapter.h"
#include "SmartMeterLineParser.h"

class Powerbaas {
  public:
    Powerbaas(bool loggingEnabled): _loggingEnabled(loggingEnabled), _smartMeterAdapter(Serial2, _smartMeterLineParser, _meterReading) {}

    void setup();
    MeterReading getMeterReading();

  private:
    bool _loggingEnabled;
    MeterReading _meterReading{0,0,0,0,0,0};
    SmartMeterLineParser _smartMeterLineParser;
    SmartMeterAdapter _smartMeterAdapter;
};

#endif
