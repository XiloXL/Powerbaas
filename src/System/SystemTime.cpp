#include <time.h>
#include "SystemTime.h"
#include "esp_sntp.h"

bool SystemTime::ntpIsSynced = false;

void time_sync_notification_cb(struct timeval *tv) {
    auto status = sntp_get_sync_status();
    if (status == SNTP_SYNC_STATUS_COMPLETED) {
        SystemTime::ntpIsSynced = true;
    }
}

void SystemTime::syncWithNTP() {
    configTime(0, 0, NTP_SERVERS);
    sntp_set_time_sync_notification_cb(time_sync_notification_cb);
}

time_t SystemTime::secondsSinceEpoch() {
    if (!ntpIsSynced) {
        return 0;
    }
    time_t result = time(nullptr);
    localtime(&result);
    return result;
}

time_t SystemTime::secondsSinceEpochTimezoned() {
    time_t utc = secondsSinceEpoch();
    TimeChangeRule *tcr; // pointer to the time change rule, use to get the TZ abbrev
    Timezone CE(CEST, CET);
    return CE.toLocal(utc, &tcr); // Central European Time (Frankfurt, Paris)
}