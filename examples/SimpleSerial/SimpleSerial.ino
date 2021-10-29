#include <Powerbaas.h>

Powerbaas powerbaas;

void setup() {
  Serial.begin(115200);
  powerbaas.setup();
}

void loop() {

  MeterReading meterReading = powerbaas.getMeterReading();

  Serial.print("Current power usage:  ");
  Serial.println(meterReading.powerUsage);

  Serial.print("Power delivered high: ");
  Serial.println(meterReading.powerDeliverHigh);
  Serial.print("Power delivered low:  ");
  Serial.println(meterReading.powerDeliverLow);

  Serial.print("Power returned high:  ");
  Serial.println(meterReading.powerReturnHigh);
  Serial.print("Power returned low:   ");
  Serial.println(meterReading.powerReturnLow);

  Serial.print("Gas used:             ");
  Serial.println(meterReading.gas);

  delay(1000);
}