#ifndef __SMART_METER_ADAPTER_H__
#define __SMART_METER_ADAPTER_H__

#define TELEGRAM_LINE_SIZE 75

#include <HardwareSerial.h>
#include "MeterReading.h"
#include "SmartMeterLineParser.h"

enum class SmartMeterState {
   idle,
   settingUpSerial,
   receiving
};

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
    void setupSerialAndBaudrate(uint8_t serialMode);
    bool isReceiving() { return _state == SmartMeterState::receiving; };

  private:

    HardwareSerial& _smartMeter;
    SmartMeterLineParser& _smartMeterLineParser;
    MeterReading& _meterReading;
    
    char _lastTelegramCharacter;
    char _telegramLine[TELEGRAM_LINE_SIZE];
    size_t _telegramIndex = 0;

    // detect baudrate and check receiving data correct
    uint8_t _serialMode = 1;
    SmartMeterState _state{SmartMeterState::idle};
};

#endif
