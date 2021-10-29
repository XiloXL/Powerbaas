#ifndef __SMART_METER_ADAPTER_H__
#define __SMART_METER_ADAPTER_H__

#define TELEGRAM_LINE_SIZE 75

#include <HardwareSerial.h>
#include "MeterReading.h"
#include "SmartMeterLineParser.h"

class SmartMeterAdapter {
  public:
    SmartMeterAdapter(
      HardwareSerial& smartMeter,
      SmartMeterLineParser& smartMeterLineParser,
      MeterReading& meterReading
    ):
      _smartMeter(smartMeter),
      _smartMeterLineParser(smartMeterLineParser),
      _meterReading(meterReading)
    {}

    void receive();
    void setupSerialAndBaudrate();
    bool isReceiving() { return _serialIsReceiving; };

  private:

    HardwareSerial& _smartMeter;
    SmartMeterLineParser& _smartMeterLineParser;
    MeterReading& _meterReading;
    
    char _lastReceivedCharacter;
    char _telegramLine[TELEGRAM_LINE_SIZE];
    size_t _dataIndex = 0;

    // detect baudrate and check receiving data correct
    uint8_t _serialMode = 1;
    bool _isSettingUpSerial = true;
    bool _serialIsReceiving = false;
};

#endif
