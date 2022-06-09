#ifndef __CREDENTIALS_H__
#define __CREDENTIALS_H__

#include <stdint.h>
#include <cstring>
#include <string>

struct UsernamePassword {
  char username[128];
  char password[128];

  bool isEmpty() const {
    return strlen(username) == 0;
  }

  void update(const std::string& newUsername, const std::string& newPassword) {
    std::size_t usernameLength = newUsername.copy(username, sizeof username - 1);
    username[usernameLength] = '\0';

    std::size_t passwordLength = newPassword.copy(password, sizeof password - 1);
    password[passwordLength] = '\0';
  }

  bool equals(const UsernamePassword& usernamePassword) const {
    return strcmp(username, usernamePassword.username) == 0 && strcmp(password, usernamePassword.password) == 0;
  }
};

struct WifiCredentials {
  UsernamePassword usernamePassword;
};

struct PowerbaasApiCredentials {
  UsernamePassword usernamePassword;
};

struct SwitchDevice433MhzCredentials {
  uint32_t address;
  uint16_t unit;
  uint16_t pulseLength;
  uint8_t iterate;
};

struct SwitchDeviceAircoCredentials {
  char urlHeat[128];    // http://192.168.2.199/control?mode=heat&temperature=20&fanspeed=medium
  char urlCool[128];    // http://192.168.2.199/control?mode=cool&temperature=16&fanspeed=low
  char urlOff[128];     // http://192.168.2.199/control?mode=off
};

struct CurrentSensorTotal {
  double total = 0.0;
};

struct CurrentSensorCalibration {
  uint8_t phases = 1;
  double calibrateUser = 1.0;
};

struct LocalApiParams {
  uint8_t deviceId;
  uint8_t type;
  char url[128];
  char paramTotal[64];
  char paramCurrent[64];
  double paramTotalMultiplier;
  double paramCurrentMultiplier;
};

#endif
