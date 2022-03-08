#include <Powerbaas.h>

Powerbaas powerbaas(true);

void setup() {
  Serial.begin(115200);

  powerbaas.onMeterReading([](const MeterReading& meterReading) {

    Serial.print("Timestamp:            ");
    Serial.println(meterReading.timestamp);

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

    Serial.println("Phases:");
    Serial.println(meterReading.voltageL1);
    Serial.println(meterReading.voltageL2);
    Serial.println(meterReading.voltageL3);

    Serial.println(meterReading.currentL1);
    Serial.println(meterReading.currentL2);
    Serial.println(meterReading.currentL3);

    Serial.println(meterReading.powerL1);
    Serial.println(meterReading.powerL2);
    Serial.println(meterReading.powerL3);
  });
  powerbaas.setup();
}

void loop() {
  powerbaas.receive();
}
