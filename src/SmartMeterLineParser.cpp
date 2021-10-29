#include "SmartMeterLineParser.h"

void SmartMeterLineParser::parse(char* line, MeterReading& meterReading) {

  float result;
  uint8_t naabPart = 0;

  // 1-0:1.8.1 = Verbruik dal
  if (sscanf(line, "1-0:1.8.1(%f", &result) == 1) {
    meterReading.powerDeliverLow = resultToInt(result);
  }

  // 1-0:1.8.2 = Verbruik piek
  else if (sscanf(line, "1-0:1.8.2(%f", &result) == 1) {
    meterReading.powerDeliverHigh = resultToInt(result);
  }

  // 1-0:2.8.1 = Teruggeleverd dal
  else if (sscanf(line, "1-0:2.8.1(%f", &result) == 1) {
    meterReading.powerReturnLow = resultToInt(result);
  }

  // 1-0:2.8.2 = Teruggeleverd piek
  else if (sscanf(line, "1-0:2.8.2(%f", &result) == 1) {
    meterReading.powerReturnHigh = resultToInt(result);
  }

  // 1-0:1.7.0 = Huidig verbruik
  // @TEST: smartMeterLineParser_IskraME382_parseCurrentPowerDeliver
  // @TEST: smartMeterLineParser_IskraAM550_parseCurrentPowerDeliver
  else if (sscanf(line, "1-0:1.7.0(%f", &result) == 1) {
    auto shortage = resultToInt(result);
    if (shortage > 0) {
      meterReading.powerUsage = shortage;
    }
  }

  // 1-0:2.7.0 = Huidig teruglever
  // @TEST: smartMeterLineParser_IskraME382_parseCurrentPowerReturn
  // @TEST: smartMeterLineParser_IskraAM550_parseCurrentPowerReturn
  else if (sscanf(line, "1-0:2.7.0(%f", &result) == 1) {
    auto oversupply = resultToInt(result);
    if (oversupply > 0) {
      meterReading.powerUsage = -oversupply;
    }
  }

  // 0-1:24.2.1 = Gas Summer
  // @TEST: smartMeterLineParser_IskraAM550_parseGasSummer
  else if (sscanf(line, "0-1:24.2.1(%ldS)(%f*m3", &naabPart, &result) == 2) {
    meterReading.gas = resultToInt(result);
  }
  // 0-1:24.2.1 = Gas Winter
  // @TEST: smartMeterLineParser_IskraAM550_parseGasWinter
  else if (sscanf(line, "0-1:24.2.1(%ldW)(%f*m3", &naabPart, &result) == 2) {
    meterReading.gas = resultToInt(result);
  }
  // Fallback Gas DSMR2.2
  // @TEST: smartMeterLineParser_IskraME382_parseGas
  else if (sscanf(line, "(%f)", &result) == 1) {
    meterReading.gas = resultToInt(result);
  }
}

uint32_t SmartMeterLineParser::resultToInt(float& result) {
    return uint32_t(result*1000);
}