#ifndef __CURRENT_SENSOR_H__
#define __CURRENT_SENSOR_H__

#include "EmonLib.h"
#include <HardwareSerial.h>
#include <SPIFFS.h>
#include "Credential.h"
#include "CredentialService.h"
#include "CalibrationTable.h"

class CurrentSensor {
public:
  CurrentSensor(
    uint32_t pullupPin,
    uint32_t pin,
    CredentialService& credentialService,
    CalibrationTable& calibrationTable)
    :
    _pullupPin(pullupPin),
    _pin(pin),
    _credentialService(credentialService),
    _calibrationTable(calibrationTable)
    {}

  bool setup();
  void setVoltage(double voltage);

  void setCalibration(const CurrentSensorCalibration& calibration);
  void storeSensorCalibration();

  void setTotal(const CurrentSensorTotal& total);
  void storeTotal();

  CurrentSensorCalibration& getCalibration();
  CurrentSensorTotal& getTotal();

  double getCurrent();
  double getSensorTotal();

private:
  uint32_t _pullupPin;
  uint32_t _pin;
  CredentialService& _credentialService;
  CalibrationTable& _calibrationTable;
  EnergyMonitor _emon;

  // calibration
  uint32_t _timeThresholdExceeded = 0;
  double _voltage = 230.0;
  uint32_t _settleTime = 5000;
  uint32_t _threshold = 50;

  // variables per loop
  uint32_t _latest = 0;

  // calibration
  CurrentSensorCalibration _currentSensorCalibration;
  CurrentSensorTotal _currentSensorTotal;

  double readWatts();
  void settle();
  bool detectSensor();
};

#endif