#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

#include "K40/alerts.h"
#include "K40/lids.h"
#include "UI/screens/status.h"
#include "queues.h"

static LidsStates lids_states;

void lids_update_status() {
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

        // Notify UI of new values
        xQueueOverwrite(lids_current_status_queue, &lids_states);
        ui_status_notify_update(STATUS_UPDATE_PROBE_LIDS);
    }
}
