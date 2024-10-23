#include <WiFi.h>
#include "ntp.h"
#include "wifi.h"

static void wifi_connected_event_handler(WiFiEvent_t event, WiFiEventInfo_t info) {
    // Initialize NTP client
    ntp_init();
}

void wifi_init() {
    WiFi.onEvent(wifi_connected_event_handler, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_CONNECTED);

    WiFi.mode(WIFI_STA);
    WiFi.setHostname(WIFI_HOSTNAME);
    WiFi.begin();
}

void wifi_connect(const char *ssid, const char *passphrase) {
    if (strlen(ssid) > 0 && strlen(passphrase) > 0) {
        WiFi.disconnect();
        WiFi.begin(ssid, passphrase);
    }
}

void wifi_disconnect() { WiFi.disconnect(); }
