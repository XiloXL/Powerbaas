#include <Powerbaas.h>

Powerbaas powerbaas(true);

void setup() {
  Serial.begin(115200);

  powerbaas.onMeterReading([](const MeterReading& meterReading) {
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
  });
  powerbaas.setup();
}

void loop() {
  powerbaas.receive();
}
