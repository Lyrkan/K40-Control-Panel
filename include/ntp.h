#ifndef NTP_H
#define NTP_H

#include <time.h>

const char NTP_SERVER[] = "pool.ntp.org";
const char NTP_TIMEZONE[] = "CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00";
const char TIME_UNKNOWN_VALUE[] = "--:--";

extern tm timeinfo;

void ntp_init();
void ntp_get_formatted_time(char output[], size_t output_size);

#endif
