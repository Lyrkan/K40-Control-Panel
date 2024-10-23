#include <time.h>
#include <WiFi.h>
#include "ntp.h"

tm timeinfo;

void ntp_init() {
    configTime(0, 0, NTP_SERVER);
    setenv("TZ", NTP_TIMEZONE, 1);
}

void ntp_get_formatted_time(char *output, size_t output_size) {
    time_t now;
    time(&now);
    localtime_r(&now, &timeinfo);

    if (timeinfo.tm_year < (2022 - 1900)) {
        snprintf(output, output_size, TIME_UNKNOWN_VALUE);
    } else {
        strftime(output, output_size, "%H:%M", &timeinfo);
    }
}
