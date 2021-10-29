#include "Powerbaas.h"

void Powerbaas::disableDebug() {
  _debug = false;
}

void Powerbaas::setup() {

  if(_debug) {
    Serial.println("");
    Serial.println("====================");
    Serial.println("Powerbaas setup:");
  }

  // try 3 different settings
  for(int i = 0; i < 3; i++) {

    if(_debug) {
      Serial.println("");
      Serial.print(" > try serial mode ");
      Serial.println(i+1);
      Serial.print("   ");
    }

    _smartMeterAdapter.setupSerialAndBaudrate();

    // try for 15 seconds
    for(int j = 0; j < 15; j++) {

      if(_debug) {
        Serial.print(".");
      }

      _smartMeterAdapter.receive();

      // we got readable data!
      if(_smartMeterAdapter.isReceiving()) {

        if(_debug) {
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
  if(_debug) {
    Serial.println("");
    Serial.println(" > setup failed!");
    Serial.println("====================");
  }
}

MeterReading Powerbaas::getMeterReading() {
  _smartMeterAdapter.receive();
  return _meterReading;
}