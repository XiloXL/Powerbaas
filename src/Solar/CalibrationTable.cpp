#include "CalibrationTable.h"

double CalibrationTable::correctWattage(double watts) {

  int16_t lowerWatt = 0;
  int16_t upperWatt = 0;
  double lowerMultiplier = 1;
  double upperMultiplier = 0;

  bool found = false;

  for(int i = 0; i < 10; i++) {
    if(watts <= _points[i]) {
      if(i > 0) {
        lowerWatt = _points[i-1];
        lowerMultiplier = _multipliers[i-1];
      }
      upperWatt = _points[i];
      upperMultiplier = _multipliers[i];
      found = true;
      break;
    }
  }

  // wattage above max value given, use last
  if(!found) {
    for(int i = 0; i < 10; i++) {
      if(_points[i] > lowerWatt) {
         upperMultiplier = _multipliers[i];
      }
    }
    return watts * upperMultiplier;
  }

  int16_t diffWatts = upperWatt - lowerWatt;
  double diffMultiplier = upperMultiplier - lowerMultiplier;

  if(diffWatts == 0 || diffMultiplier == 0) {
     return watts;
  }

  double multiplier = lowerMultiplier + (diffMultiplier * (watts - lowerWatt) / diffWatts);

  return watts * multiplier;
}