#include "CurrentSensor.h"

bool CurrentSensor::setup() {

  // threshold
  _latest = millis();
  _timeThresholdExceeded = 0;

  // load calibration & total from spiffs
  _credentialService.load(_currentSensorCalibration);   // phases, user calibration
  _credentialService.load(_currentSensorTotal);         // meter reading total

  // setup
  analogReadResolution(10);
  pinMode(_pullupPin, INPUT_PULLUP);
  _emon.current(_pin, 30);

  // detect sensor
  bool sensorDetected = detectSensor();
  pinMode(_pullupPin, INPUT);
  if(sensorDetected == false) {
    return false;
  }

  // settle
  settle();

  return true;
}

void CurrentSensor::setVoltage(double voltage) {
  if(voltage > 200 && voltage < 275) {
    _voltage = voltage;
  }
}

void CurrentSensor::settle() {
  // wait 5 sec for sensor to settle
  uint32_t calibrationStart = millis();
  uint32_t calibrationStop = calibrationStart + _settleTime;
  while (millis() < calibrationStop) {
    double watts = readWatts();
  }
}

bool CurrentSensor::detectSensor() {
  if(analogRead(_pin) == 1023) {
    return false;
  } else {
    return true;
  }
}

void CurrentSensor::setCalibration(const CurrentSensorCalibration& calibration) {
  _currentSensorCalibration = calibration;
  storeSensorCalibration();
}

void CurrentSensor::setTotal(const CurrentSensorTotal& total) {
  _currentSensorTotal = total;
  storeTotal();
}

void CurrentSensor::storeTotal() {
  if(_currentSensorTotal.total == 0) {
    return;
  }
  _credentialService.save(_currentSensorTotal);
}

void CurrentSensor::storeSensorCalibration() {
  _credentialService.save(_currentSensorCalibration);
}

CurrentSensorCalibration& CurrentSensor::getCalibration() {
  return _currentSensorCalibration;
}

CurrentSensorTotal& CurrentSensor::getTotal() {
  return _currentSensorTotal;
}

double CurrentSensor::getSensorTotal() {
  return _currentSensorTotal.total;
}

double CurrentSensor::getCurrent() {

  // calculate time between 2 readings
  uint32_t now = millis();
  uint32_t durationInMillis = 0;
  if(_latest > 0) {
    durationInMillis = now - _latest;
  }
  _latest = now;

  double watts = readWatts();

  // filter 1: user editable multiplier
  watts = (watts * _currentSensorCalibration.calibrateUser);

  // multiple phases?
  if(_currentSensorCalibration.phases > 1) {
    watts *= _currentSensorCalibration.phases;
  }

  // filter 2: calibration table
  watts = _calibrationTable.correctWattage(watts);

  // filter 3: reactive power
  //watts = _filterReactivePower.filterReactivePower((int16_t)watts, millis());

  // filter 4: watts below threshold, cutoff!
  if(watts < 50) {
    _timeThresholdExceeded = 0;
    return 0;
  }

  // filter 5: filters noise around 0-point, at least 10 seconds threshold met!
  _timeThresholdExceeded += durationInMillis;
  if(_timeThresholdExceeded < 10000) {
    return 0;
  }

  // watts to wattsecond
  double wattsec = (watts * durationInMillis) / 1000.0;

  // wattsecond to watthour
  double watthour = wattsec * 0.000278;

  // add to total
  _currentSensorTotal.total += watthour;

  return watts;
}

double CurrentSensor::readWatts() {
  analogReadResolution(10);
  double Irms = _emon.calcIrms(5588);//5588,1480);
  double watts = (Irms * _voltage);
  return watts;
}
