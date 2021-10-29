#include "SmartMeterAdapter.h"

// called from Powerbaas every 15seconds
void SmartMeterAdapter::setupSerialAndBaudrate() {
  if(!_serialIsReceiving) {

    _isSettingUpSerial = true;

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

    _isSettingUpSerial = false;

    _serialMode++;
    if(_serialMode > 2) {
      _serialMode = 0;
    }
  }
}

void SmartMeterAdapter::receive() {

  if(_isSettingUpSerial) {
    return;
  }

  while (_smartMeter.available() > 0) {

    _lastReceivedCharacter = _smartMeter.read();

    // Fill buffer up to and including a new line (\n)
    _telegramLine[_dataIndex] = _lastReceivedCharacter&127;
    _dataIndex++;

    if (_dataIndex >= TELEGRAM_LINE_SIZE) {
      _dataIndex = 0;
    }

    // do nothing until we get a \n
    if (_lastReceivedCharacter == '\n') {

      // print out complete line
      //Serial.print(_telegramLine);

      // Parse telegram line
      _smartMeterLineParser.parse(_telegramLine, _meterReading);

      // Bericht eindigt met !...
      if (_telegramLine[0] == '!') {

        // Data readable!
        _serialIsReceiving = true;
      }

      // Empty buffer again (whole array)
      for (size_t i = 0; i < TELEGRAM_LINE_SIZE; i++) {
        _telegramLine[i] = 0;
      }
      _dataIndex = 0;
    }
  }
}