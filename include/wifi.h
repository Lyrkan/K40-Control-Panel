#ifndef WIFI_H
#define WIFI_H

const char WIFI_HOSTNAME[] = "K40-Control-Panel";

void wifi_init();
void wifi_connect(const char *ssid, const char *passphrase);
void wifi_disconnect();

#endif
