#include "Powerbaas.h"

void Powerbaas::setup() {

  if(_loggingEnabled) {
    Serial.println("");
    Serial.println("====================");
    Serial.println("Powerbaas setup:");
  }

  // try 3 different settings
  for (int serialMode = 0; serialMode < 3; serialMode++) {

    if(_loggingEnabled) {
      Serial.println("");
      Serial.print(" > try serial mode ");
      Serial.println(serialMode+1);
      Serial.print("   ");
    }

    _smartMeterAdapter.setupSerialAndBaudrate(serialMode);

    // try for 15 seconds
    for(int seconds = 0; seconds < 15; seconds++) {

      if(_loggingEnabled) {
        Serial.print(".");
      }

      _smartMeterAdapter.receive();

      // we got readable data!
      if(_smartMeterAdapter.isReceiving()) {

        if(_loggingEnabled) {
          Serial.println("");
          Serial.println(" > setup complete!");
          Serial.println("====================");
        }

        return;
      }
      // wait 1 second
      delay(1000);
    }
  }

  // failed!
  if(_loggingEnabled) {
    Serial.println("");
    Serial.println(" > setup failed!");
    Serial.println("====================");
  }
}

MeterReading Powerbaas::getMeterReading() {
  _smartMeterAdapter.receive();
  return _meterReading;
}
