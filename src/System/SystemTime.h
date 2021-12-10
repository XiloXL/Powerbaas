#ifndef __SYSTEM_TIME_H__
#define __SYSTEM_TIME_H__

#include <WString.h>
#define NTP_SERVERS "nl.pool.ntp.org", "pool.ntp.org", "time.nist.gov"

#include "../Timezone/Timezone.h"

class SystemLogger;

class SystemTime {
public:
    void syncWithNTP();
    time_t secondsSinceEpoch();
    time_t secondsSinceEpochTimezoned();

    //may only be set by this class and the callback for ntp
    static bool ntpIsSynced;
private:
    // Timezone Central European Time
    TimeChangeRule CEST = {"CEST", Last, Sun, Mar, 2, 120};     // Central European Summer Time
    TimeChangeRule CET = {"CET ", Last, Sun, Oct, 3, 60};       // Central European Standard Time
};

#endif
