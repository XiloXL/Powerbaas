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

struct CurrentSensorTotal {
  double total = 0.0;
};

struct CurrentSensorCalibration {
  uint8_t phases = 1;
  double calibrateUser = 1.0;
};

#endif
