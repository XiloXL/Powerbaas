#include "SmartMeterAdapter.h"

// called from Powerbaas every 15seconds
void SmartMeterAdapter::setupSerialAndBaudrate(uint8_t serialMode) {
  if (_state == SmartMeterState::receiving) {
    return;
  }

  _state = SmartMeterState::settingUpSerial;
  _serialMode = serialMode;

  _smartMeter.end();
  delay(100);

  if(_serialMode == 0) {
    _smartMeter.begin(115200, SERIAL_8N1, 16, 17, true);
  } else if(_serialMode == 1) {
    _smartMeter.begin(9600, SERIAL_7E1, 16, 17, true);
  } else if(_serialMode == 2) {
    _smartMeter.begin(115200, SERIAL_7E1, 16, 17, true);
  }

  // default 256, need more for longer lines!
  _smartMeter.setRxBufferSize(1024);

  _state = SmartMeterState::idle;
}

void SmartMeterAdapter::receive() {

  if (_state == SmartMeterState::settingUpSerial) {
    return;
  }

  while (_smartMeter.available() > 0) {

    _lastTelegramCharacter = _smartMeter.read();

    // Fill buffer up to and including a new line (\n)
    _telegramLine[_telegramIndex] = _lastTelegramCharacter&127;
    _telegramIndex++;

    if (_telegramIndex >= TELEGRAM_LINE_SIZE) {
      _telegramIndex = 0;
    }

    // do nothing until we get a \n
    if (_lastTelegramCharacter == '\n') {
      // Parse telegram line
      _smartMeterLineParser.parse(_telegramLine, _meterReading);

      // Message ends with !
      if (_telegramLine[0] == '!') {

        // Data readable!
        _state = SmartMeterState::receiving;
      }

      // Empty buffer again (whole array)
      for (size_t i = 0; i < TELEGRAM_LINE_SIZE; i++) {
        _telegramLine[i] = 0;
      }
      _telegramIndex = 0;
    }
  }
}
