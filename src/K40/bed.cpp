#include <Arduino.h>
#include <math.h>

#include "K40/bed.h"
#include "queues.h"
#include "settings.h"

static hw_timer_t *bed_step_interrupt_timer = NULL;
static volatile uint32_t bed_step_interrupt_remaining_steps = 0;
static volatile BedDirection bed_current_direction = BED_DIR_UNKNOWN;
static volatile int32_t bed_isr_step_delta_nm = 0;
static portMUX_TYPE bed_mux = portMUX_INITIALIZER_UNLOCKED;
static volatile BedStatus bed_current_status = {
    .state = BED_STATE_IDLE,
    .target = {.is_set = false, .position_nm = 0},
    .current = {.is_set = false, .position_nm = 0},
    .origin = {.is_set = false, .position_nm = 0}};

void IRAM_ATTR bed_step_interrupt() {
    if ((bed_current_direction == BED_DIR_UNKNOWN) || (bed_step_interrupt_remaining_steps == 0) ||
        ((digitalRead(PIN_BED_LIMIT) == BED_LIMIT_PIN_ACTIVATION_STATE) && bed_current_direction != BED_DIR_UP)) {
        bed_step_interrupt_remaining_steps = 0;
        return;
    }

    // Do a step
    digitalWrite(PIN_BED_STEP, HIGH);
    digitalWrite(PIN_BED_STEP, LOW);
    bed_step_interrupt_remaining_steps--;

    portENTER_CRITICAL_ISR(&bed_mux);
    bed_current_status.current.position_nm += bed_isr_step_delta_nm;
    portEXIT_CRITICAL_ISR(&bed_mux);
}

static void bed_update_status_queue() {
    portENTER_CRITICAL(&bed_mux);
    BedStatus bed_current_status_copy = {
        .state = bed_current_status.state,
        .target = {.is_set = bed_current_status.target.is_set, .position_nm = bed_current_status.target.position_nm},
        .current = {.is_set = bed_current_status.current.is_set, .position_nm = bed_current_status.current.position_nm},
        .origin = {.is_set = bed_current_status.origin.is_set, .position_nm = bed_current_status.origin.position_nm}};
    portEXIT_CRITICAL(&bed_mux);

    xQueueOverwrite(bed_current_status_queue, &bed_current_status_copy);
}

static void bed_run_steps(uint32_t steps, BedDirection direction, uint32_t steps_per_second) {
    uint32_t step_divisor = bed_settings.steps_per_revolution * bed_settings.microstep_multiplier;
    if (step_divisor == 0 || steps_per_second == 0) {
        log_e("Bed: invalid settings (step_divisor=%u, steps_per_second=%u)", step_divisor, steps_per_second);
        return;
    }

    // Cache motion parameter for ISR use
    bed_isr_step_delta_nm =
        (1000 * direction * (int32_t)bed_settings.screw_lead_um) / (int32_t)step_divisor;

    // Update direction
    bed_current_direction = direction;
    digitalWrite(PIN_BED_DIR, direction > 0 ? HIGH : LOW);

    // Enable the stepper timer interrupt
    unsigned long interval_between_steps_micros = 1000000 / steps_per_second;
    bed_step_interrupt_remaining_steps = steps;
    timerAlarmWrite(bed_step_interrupt_timer, interval_between_steps_micros, true);
    timerAlarmEnable(bed_step_interrupt_timer);
}

static void bed_stop() {
    // Do not stop the timer here, or state will not be updated.
    // The interrupt will be called one last time before
    // the state is set to idle by the bed_run function.
    bed_step_interrupt_remaining_steps = 0;
}

static void bed_home() {
    // Run the max possible amount of steps down
    // hoping for the limit switch to be triggered.
    bed_run_steps(UINT32_MAX, BED_DIR_DOWN, bed_settings.homing_speed);

    // Set new state
    portENTER_CRITICAL(&bed_mux);
    bed_current_status.state = BED_STATE_HOMING;
    bed_current_status.current.is_set = false;
    bed_current_status.target.is_set = false;
    portEXIT_CRITICAL(&bed_mux);
    bed_update_status_queue();
}

static void bed_move_relative(float_t value_nm) {
    uint32_t step_divisor = bed_settings.steps_per_revolution * bed_settings.microstep_multiplier;
    if (step_divisor == 0) {
        log_e("Bed: invalid settings (step_divisor=0)");
        return;
    }

    // Start the stepper with the amount of steps required
    uint32_t steps_to_run =
        (uint32_t)fabsf(value_nm / ((1000.0f * bed_settings.screw_lead_um) / step_divisor));
    bed_run_steps(steps_to_run, value_nm < 0 ? BED_DIR_DOWN : BED_DIR_UP, bed_settings.moving_speed);

    // Set new state
    portENTER_CRITICAL(&bed_mux);
    bed_current_status.state = value_nm < 0 ? BED_STATE_GOING_DOWN : BED_STATE_GOING_UP;
    bed_current_status.target.is_set = true;
    bed_current_status.target.position_nm = bed_current_status.current.position_nm + value_nm;
    portEXIT_CRITICAL(&bed_mux);
    bed_update_status_queue();
}

static void bed_move_absolute(float_t value_nm) {
    portENTER_CRITICAL(&bed_mux);
    bool current_is_set = bed_current_status.current.is_set;
    int32_t current_pos = bed_current_status.current.position_nm;
    bool origin_is_set = bed_current_status.origin.is_set;
    int32_t origin_pos = bed_current_status.origin.position_nm;
    portEXIT_CRITICAL(&bed_mux);

    if (!current_is_set) {
        return;
    }

    int32_t target_absolute_delta_nm = value_nm - current_pos;
    if (origin_is_set) {
        target_absolute_delta_nm += origin_pos;
    }

    bed_move_relative(target_absolute_delta_nm);
}

static void bed_set_current_position_as_origin() {
    portENTER_CRITICAL(&bed_mux);
    bool current_is_set = bed_current_status.current.is_set;
    int32_t current_pos = bed_current_status.current.position_nm;
    portEXIT_CRITICAL(&bed_mux);

    if (!current_is_set) {
        return;
    }

    portENTER_CRITICAL(&bed_mux);
    bed_current_status.origin.position_nm = current_pos;
    bed_current_status.origin.is_set = true;
    portEXIT_CRITICAL(&bed_mux);

    bed_update_status_queue();

    // Save origin into configuration
    bed_settings.origin = {
        .is_set = true,
        .position_nm = current_pos};
    settings_schedule_save(SETTINGS_TYPE_BED);
}

static void bed_run() {
    // Check if there is still distance to go
    if ((bed_current_status.state != BED_STATE_IDLE) && (bed_step_interrupt_remaining_steps == 0)) {
        // Disable the interrupt timer
        timerAlarmDisable(bed_step_interrupt_timer);

        // Check if we reached the limit switch
        if (digitalRead(PIN_BED_LIMIT) == BED_LIMIT_PIN_ACTIVATION_STATE &&
            bed_current_status.state != BED_STATE_GOING_UP) {
            log_i("Bed limit switch triggered");
            portENTER_CRITICAL(&bed_mux);
            bed_current_status.current.is_set = true;
            bed_current_status.current.position_nm = 0;
            portEXIT_CRITICAL(&bed_mux);

            // Move up to make sure the limit switch is not triggered anymore
            bed_move_relative(bed_settings.backoff_distance_um * 1000);
        } else {
            // Go to idling mode
            portENTER_CRITICAL(&bed_mux);
            bed_current_status.state = BED_STATE_IDLE;
            bed_current_status.target.is_set = false;
            portEXIT_CRITICAL(&bed_mux);
            bed_update_status_queue();
        }

        return;
    }

    // Notify the UI every BED_RUNNING_UPDATE_INTERVAL milliseconds
    unsigned long current_time = millis();
    static unsigned long last_update_time = 0;
    if ((current_time - last_update_time) >= BED_RUNNING_UPDATE_INTERVAL) {
        last_update_time = current_time;
        bed_update_status_queue();
    }
}

void bed_init() {
    // Load origin from configuration
    bed_current_status.origin.is_set = bed_settings.origin.is_set;
    bed_current_status.origin.position_nm = bed_settings.origin.position_nm;
    bed_update_status_queue();

    // Create the stepper interrupt timer
    // Its speed will be set by bed_set_speed() and started by bed_run_steps()
    bed_step_interrupt_timer = timerBegin(BED_STEP_TIMER_ID, APB_CLK_FREQ / 1000000, true);
    timerAttachInterrupt(bed_step_interrupt_timer, &bed_step_interrupt, true);
}

BedState bed_update() {
    // Process new command if there is one
    static BedCommand bed_command;
    if (xQueueReceive(bed_command_queue, &bed_command, 0) == pdTRUE) {
        switch (bed_command.type) {
        case BED_COMMAND_STOP:
            log_i("Bed: Received STOP command");
            bed_stop();
            break;
        case BED_COMMAND_HOME:
            log_i("Bed: Received HOME command");
            bed_home();
            break;
        case BED_COMMAND_MOVE_ABSOLUTE:
            log_i("Bed: Received MOVE_ABSOLUTE command (value_nm=%d)", bed_command.value_nm);
            bed_move_absolute(bed_command.value_nm);
            break;
        case BED_COMMAND_MOVE_RELATIVE:
            log_i("Bed: Received MOVE_RELATIVE command (value_nm=%d)", bed_command.value_nm);
            bed_move_relative(bed_command.value_nm);
            break;
        case BED_COMMAND_SET_CURRENT_POSITION_AS_ORIGIN:
            log_i("Bed: Received SET_CURRENT_POSITION_AS_ORIGIN command");
            bed_set_current_position_as_origin();
            break;
        }
    }

    // If bed isn't idling execute its current action
    if (bed_current_status.state != BED_STATE_IDLE) {
        bed_run();
    }

    return bed_current_status.state;
}
