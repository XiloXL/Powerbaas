#include "SmartMeterLineParser.h"

void SmartMeterLineParser::parse(char* line, MeterReading& meterReading) {

  float result;

  // belgium
  // @TEST: smartMeterLineParser_Netherlands
  // @TEST: smartMeterLineParser_Belgium
  char country[3];
  if (sscanf(line, "/%3s", &country) == 1) {
    if (strcmp(country, "FLU") == 0) {
      _isBelgium = true;
    }
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

  // parse country differences
  if(_isBelgium) {
    parseBelgium(line, meterReading);
  } else {
    parseNetherlands(line, meterReading);
  }
}

void SmartMeterLineParser::parseNetherlands(char* line, MeterReading& meterReading) {

  float result;
  uint8_t discardGas1 = 0;
  uint8_t discardGas2 = 0;
  uint8_t discardGas3 = 0;

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

  // 0-?:24.2.1 = Gas meter 1 tm 4 - Summer/Winter
  // @TEST: smartMeterLineParser_IskraAM550_parseGasWinter
  // @TEST: smartMeterLineParser_IskraAM550_parseGasSummer
  // @TEST: smartMeterLineParser_IskraAM550_parseGasSecond
  else if (sscanf(line, "0-%u:24.2.1(%ld%c)(%f*m3", &discardGas1, &discardGas2, &discardGas3, &result) == 4) {
    meterReading.gas = resultToInt(result);
  }

  // Fallback Gas DSMR2.2
  // @TEST: smartMeterLineParser_IskraME382_parseGas
  else if (sscanf(line, "(%f)", &result) == 1) {
    meterReading.gas = resultToInt(result);
  }
}

void SmartMeterLineParser::parseBelgium(char* line, MeterReading& meterReading) {

  float result;
  uint8_t discardGas1 = 0;
  uint8_t discardGas2 = 0;
  uint8_t discardGas3 = 0;

  // 1-0:1.8.1 = Verbruik piek
  // @TEST: smartMeterLineParser_Belgium_parseHighTariff
  if (sscanf(line, "1-0:1.8.1(%f", &result) == 1) {
    meterReading.powerDeliverHigh = resultToInt(result);
  }

  // 1-0:1.8.2 = Verbruik dal
  else if (sscanf(line, "1-0:1.8.2(%f", &result) == 1) {
    meterReading.powerDeliverLow = resultToInt(result);
  }

  // 1-0:2.8.1 = Injectie piek
  else if (sscanf(line, "1-0:2.8.1(%f", &result) == 1) {
    meterReading.powerReturnHigh = resultToInt(result);
  }

  // 1-0:2.8.2 = Injectie dal
  else if (sscanf(line, "1-0:2.8.2(%f", &result) == 1) {
    meterReading.powerReturnLow = resultToInt(result);
  }

  // 0-?:24.2.1 = Gas meter 1 tm 4 - Summer/Winter
  // @TEST: smartMeterLineParser_Belgium_parseGas
  else if (sscanf(line, "0-%u:24.2.3(%ld%c)(%f*m3", &discardGas1, &discardGas2, &discardGas3, &result) == 4) {
    meterReading.gas = resultToInt(result);
  }
}

uint32_t SmartMeterLineParser::resultToInt(float& result) {
    return uint32_t(result*1000);
}