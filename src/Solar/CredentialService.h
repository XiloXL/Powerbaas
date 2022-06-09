#ifndef __CREDENTIAL_SERVICE_H__
#define __CREDENTIAL_SERVICE_H__

#include <SPIFFS.h>
#include "Credential.h"

class CredentialRepository {
public:
    template<typename T>
    void save(const char* filename, const T& credential) {
        SPIFFS.remove(filename);
        File myFile = SPIFFS.open(filename, FILE_WRITE);
        myFile.write((byte *)&credential, sizeof(credential));
        myFile.close();
    }

    template<typename T>
    void load(const char* filename, T& credential) {
        if(SPIFFS.exists(filename) == false) {
            return;
        }
        File myFile = SPIFFS.open(filename);
        if (!myFile) {
            return;
        }
        myFile.read((byte *)&credential, sizeof(credential));
        myFile.close();
    }
private:
};

class CredentialService {
public:
    CredentialService(const char* basePath, CredentialRepository& credentialRepository): _basePath(basePath), _credentialRepository(credentialRepository) {}

    void save(const CurrentSensorCalibration credentials) {
        String file = String(_basePath) + "current-sensor.bin";
        _credentialRepository.save(file.c_str(), credentials);
    }
    void load(CurrentSensorCalibration& credentials) {
        String file = String(_basePath) + "current-sensor.bin";
        _credentialRepository.load(file.c_str(), credentials);
    }

    // current sensor meter reading
    void save(const CurrentSensorTotal& credentials) {
        String file = String(_basePath) + "current-sensor-total.bin";
        _credentialRepository.save(file.c_str(), credentials);
    }
    void load(CurrentSensorTotal& credentials) {
        String file = String(_basePath) + "current-sensor-total.bin";
        _credentialRepository.load(file.c_str(), credentials);
    }

private:
    CredentialRepository& _credentialRepository;
    const char* _basePath = "/";
};

#endif
