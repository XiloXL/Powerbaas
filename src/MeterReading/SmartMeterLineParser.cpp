#include "SmartMeterLineParser.h"

void SmartMeterLineParser::parse(char* line, MeterReading& meterReading) {

  float result;
  char tms[14];

  // belgium
  // @TEST: smartMeterLineParser_Netherlands
  // @TEST: smartMeterLineParser_Belgium
  char country[3];
  if (sscanf(line, "/%3s", &country) == 1) {
    if (strcmp(country, "FLU") == 0) {
      _isBelgium = true;
    }
  }

  // 0-0:1.0.0 = Timestamp
  else if (sscanf(line, "0-0:1.0.0(%13s", &tms) == 1) {
    strcpy(meterReading.timestamp, tms);
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


  // Amps line 1 / 3
  else if (sscanf(line, "1-0:31.7.0(%f*A", &result) == 1) {
    meterReading.currentL1 = result;
  }
  else if (sscanf(line, "1-0:51.7.0(%f*A", &result) == 1) {
    meterReading.currentL2 = result;
  }
  else if (sscanf(line, "1-0:71.7.0(%f*A", &result) == 1) {
    meterReading.currentL3 = result;
  }

  // Voltages line 1 / 3
  else if (sscanf(line, "1-0:32.7.0(%f*V", &result) == 1) {
    meterReading.voltageL1 = result;
  }
  else if (sscanf(line, "1-0:52.7.0(%f*V", &result) == 1) {
    meterReading.voltageL2 = result;
  }
  else if (sscanf(line, "1-0:72.7.0(%f*V", &result) == 1) {
    meterReading.voltageL3 = result;
  }

  // Power line 1 / 3
  else if (sscanf(line, "1-0:21.7.0(%f", &result) == 1) {
    auto shortage = resultToInt(result);
    if (shortage > 0) {
      meterReading.powerL1 = shortage;
    }
  }
  else if (sscanf(line, "1-0:22.7.0(%f", &result) == 1) {
    auto oversupply = resultToInt(result);
    if (oversupply > 0) {
      meterReading.powerL1 = -oversupply;
    }
  }
  else if (sscanf(line, "1-0:41.7.0(%f", &result) == 1) {
    auto shortage = resultToInt(result);
    if (shortage > 0) {
      meterReading.powerL2 = shortage;
    }
  }
  else if (sscanf(line, "1-0:42.7.0(%f", &result) == 1) {
    auto oversupply = resultToInt(result);
    if (oversupply > 0) {
      meterReading.powerL2 = -oversupply;
    }
  }
  else if (sscanf(line, "1-0:61.7.0(%f", &result) == 1) {
    auto shortage = resultToInt(result);
    if (shortage > 0) {
      meterReading.powerL3 = shortage;
    }
  }
  else if (sscanf(line, "1-0:62.7.0(%f", &result) == 1) {
    auto oversupply = resultToInt(result);
    if (oversupply > 0) {
      meterReading.powerL3 = -oversupply;
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

  // MBUS  devices
  uint8_t mbusDevice = 0;
  uint8_t mbusDeviceType = 0;
  if (sscanf(line, "0-%u:24.1.0(%u)", &mbusDevice, &mbusDeviceType) == 2) {
    _currentMbusDevice = mbusDevice;
    _currentMbusDeviceType = mbusDeviceType;
  }

  // 0-?:24.2.1 = MBUS Gas meter 1 tm 4 - Summer/Winter
  uint8_t discardGas2 = 0;
  uint8_t discardGas3 = 0;
  if (sscanf(line, "0-%u:24.2.1(%ld%c)(%lf*m3", &mbusDevice, &discardGas2, &discardGas3, &result) == 4) {
    if(mbusDevice == _currentMbusDevice && _currentMbusDeviceType == 3) {
        meterReading.gas = resultToInt(result);
        _currentMbusDeviceType = 0;
    }
    return;
  }

  // Fallback Gas DSMR2.2
  // @TEST: smartMeterLineParser_IskraME382_parseGas
  if (sscanf(line, "(%lf)", &result) == 1) {
    meterReading.gas = resultToInt(result);
    return;
  }
}

void SmartMeterLineParser::parseBelgium(char* line, MeterReading& meterReading) {

  float result;

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

  // MBUS  devices
  uint8_t mbusDevice = 0;
  uint8_t mbusDeviceType = 0;
  if (sscanf(line, "0-%u:24.1.0(%u)", &mbusDevice, &mbusDeviceType) == 2) {
    _currentMbusDevice = mbusDevice;
    _currentMbusDeviceType = mbusDeviceType;
  }

  // 0-?:24.2.1 = Gas meter 1 tm 4 - Summer/Winter
  // @TEST: smartMeterLineParser_Belgium_parseGas
  uint8_t discardGas2 = 0;
  uint8_t discardGas3 = 0;
  if (sscanf(line, "0-%u:24.2.3(%ld%c)(%lf*m3", &mbusDevice, &discardGas2, &discardGas3, &result) == 4) {
    if(mbusDevice == _currentMbusDevice && _currentMbusDeviceType == 3) {
      meterReading.gas = resultToInt(result);
      _currentMbusDeviceType = 0;
    }
    return;
  }
}

uint32_t SmartMeterLineParser::resultToInt(float& result) {
    return uint32_t(result*1000);
}
