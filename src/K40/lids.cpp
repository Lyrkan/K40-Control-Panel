#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

#include "K40/alerts.h"
#include "K40/lids.h"
#include "macros.h"
#include "mutex.h"
#include "queues.h"

#if HAS_DISPLAY
#include "UI/screens/status.h"
#else
#include "Headless/headless_serial.h"
#endif

LidsStates lids_states;

void lids_update_status() {
    TAKE_MUTEX(lids_current_status_mutex);
    static bool first_update = true;
    bool front_lid_opened = digitalRead(PIN_LID_STATUS_FRONT) == HIGH;
    bool back_lid_opened = digitalRead(PIN_LID_STATUS_BACK) == HIGH;

    if (first_update || front_lid_opened != lids_states.front_opened || back_lid_opened != lids_states.back_opened) {
        first_update = false;
        lids_states.front_opened = front_lid_opened;
        lids_states.back_opened = back_lid_opened;

        // Change alert state
        bool enable_alert = front_lid_opened || back_lid_opened;
        alerts_toggle_alert(ALERT_TYPE_LIDS, enable_alert);

#if HAS_DISPLAY
        // Notify UI of new values
        ui_status_notify_update(STATUS_UPDATE_PROBE_LIDS);
#else
        headless_notify_update(HEADLESS_UPDATE_STATUS);
#endif
    }
    RELEASE_MUTEX(lids_current_status_mutex);
}
