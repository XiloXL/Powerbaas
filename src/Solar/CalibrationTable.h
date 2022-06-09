#ifndef __CALIBRATION_TABLE_H__
#define __CALIBRATION_TABLE_H__

#include "CredentialService.h"

class CalibrationTable {
public:
  CalibrationTable(){}

  double correctWattage(double watts);

private:
  uint16_t _points[10] = {1,500,0,0,0,0,0,0,0,0};
  double _multipliers[10] = {0.001,1.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
};

#endif