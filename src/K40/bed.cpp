#include <AccelStepper.h>
#include <math.h>

#include "K40/bed.h"
#include "queues.h"
#include "settings.h"

static AccelStepper stepper(AccelStepper::DRIVER, PIN_BED_STEP, PIN_BED_DIR);

static BedStatus bed_current_status = {
    .state = BED_STATE_IDLE,
    .target = {.is_set = false, .position = 0},
    .current = {.is_set = false, .position = 0},
    .origin = {.is_set = false, .position = 0}};

static void bed_run() {
    // Run the stepper
    stepper.run();

    // Update position and idle if the distance to go is reached
    bed_current_status.current.position =
        ((float_t)stepper.currentPosition() / (bed_settings.steps_per_revolution * bed_settings.microstep_multiplier)) *
        bed_settings.screw_pitch;
    if (stepper.distanceToGo() == 0) {
        bed_current_status.state = BED_STATE_IDLE;
        bed_current_status.target.is_set = false;
    }

    xQueueOverwrite(bed_current_status_queue, &bed_current_status);
}

static void bed_stop() {
    // Perform an instant stop
    stepper.setCurrentPosition(stepper.currentPosition());

    // Set new state
    bed_current_status.state = BED_STATE_IDLE;
    bed_current_status.target.is_set = false;

    xQueueOverwrite(bed_current_status_queue, &bed_current_status);
}

static void bed_home() {
    // Perform an instant stop
    stepper.setCurrentPosition(stepper.currentPosition());

    // Set new state
    bed_current_status.state = BED_STATE_HOMING;
    bed_current_status.current.is_set = false;
    bed_current_status.target.is_set = false;

    // Move down using an arbitrary large number, the stepper
    // should stop once it reaches the limit switch.
    stepper.setMaxSpeed(bed_settings.homing_speed * bed_settings.microstep_multiplier);
    stepper.move(-1000 * (bed_settings.steps_per_revolution * bed_settings.microstep_multiplier));
}

static void bed_move_relative(float_t value) {
    // Perform an instant stop
    stepper.setCurrentPosition(stepper.currentPosition());

    // Set new state
    bed_current_status.state = value < 0 ? BED_STATE_GOING_DOWN : BED_STATE_GOING_UP;
    if (bed_current_status.current.is_set) {
        bed_current_status.target.is_set = true;
        bed_current_status.target.position = bed_current_status.current.position + value;
    }

    stepper.setMaxSpeed(bed_settings.moving_speed * bed_settings.microstep_multiplier);
    stepper.move(
        (value / bed_settings.screw_pitch) * (bed_settings.steps_per_revolution * bed_settings.microstep_multiplier));
}

static void bed_move_absolute(float_t value) {
    // If the current position is not set the bed
    // can't do absolute moves.
    if (!bed_current_status.current.is_set) {
        return;
    }

    // Perform a quick stop
    stepper.setCurrentPosition(stepper.currentPosition());

    // Set new state
    bed_current_status.state = value < bed_current_status.current.position ? BED_STATE_GOING_DOWN : BED_STATE_GOING_UP;
    bed_current_status.target.is_set = true;
    bed_current_status.target.position = bed_current_status.target.position;

    stepper.setMaxSpeed(bed_settings.moving_speed * bed_settings.microstep_multiplier);
    stepper.moveTo(
        (value / bed_settings.screw_pitch) * (bed_settings.steps_per_revolution * bed_settings.microstep_multiplier));
}

static void bed_set_current_position_as_origin() {
    if (!bed_current_status.current.is_set) {
        bed_current_status.current.is_set = true;
        bed_current_status.current.position = 0;
    }

    bed_current_status.origin.is_set = true;
    bed_current_status.origin.position += bed_current_status.current.position;
    bed_current_status.current.position = 0;
    stepper.setCurrentPosition(0);

    xQueueOverwrite(bed_current_status_queue, &bed_current_status);

    // Save origin into configuration
    bed_settings.origin = bed_current_status.origin;
    settings_schedule_save(SETTINGS_TYPE_BED);
}

void bed_init() {
    stepper.setMaxSpeed(bed_settings.moving_speed * bed_settings.microstep_multiplier);
    stepper.setAcceleration(bed_settings.acceleration * bed_settings.microstep_multiplier);

    // Load origin from configuration
    bed_current_status.origin = bed_settings.origin;

    xQueueOverwrite(bed_current_status_queue, &bed_current_status);
}

BedState bed_update() {
    // Process new command if there is one
    static BedCommand bed_command;
    if (xQueueReceive(bed_command_queue, &bed_command, 0) == pdTRUE) {
        switch (bed_command.type) {
        case BED_COMMAND_STOP:
            Serial.println("Bed: Received STOP command");
            bed_stop();
            break;
        case BED_COMMAND_HOME:
            Serial.println("Bed: Received HOME command");
            bed_home();
            break;
        case BED_COMMAND_MOVE_ABSOLUTE:
            Serial.println("Bed: Received MOVE_ABSOLUTE command");
            bed_move_absolute(bed_command.value);
            break;
        case BED_COMMAND_MOVE_RELATIVE:
            Serial.println("Bed: Received MOVE_RELATIVE command");
            bed_move_relative(bed_command.value);
            break;
        case BED_COMMAND_SET_CURRENT_POSITION_AS_ORIGIN:
            Serial.println("Bed: Received SET_CURRENT_POSITION_AS_ORIGIN command");
            bed_set_current_position_as_origin();
            break;
        }
    }

    // If bed is idling wait a bit before the next command check
    if (bed_current_status.state != BED_STATE_IDLE) {
        // Check if the limit switch is triggered.
        // If that's the case stop the stepper and start idling.
        if (digitalRead(PIN_BED_LIMIT) == LOW) {
            // If the bed was homing and the origin is known
            // update the current position
            Serial.println("Bed: Limit switch triggered");
            if (bed_current_status.state == BED_STATE_HOMING) {
                bed_current_status.current.is_set = true;
                if (bed_current_status.origin.is_set) {
                    bed_current_status.current.position = bed_current_status.origin.position;
                    stepper.setCurrentPosition(
                        bed_current_status.origin.position * bed_settings.screw_pitch *
                        (bed_settings.steps_per_revolution * bed_settings.microstep_multiplier));
                } else {
                    bed_current_status.current.position = 0;
                    bed_current_status.origin.position = bed_current_status.current.position;
                    bed_current_status.origin.is_set = true;
                    stepper.setCurrentPosition(0);
                }
            }

            bed_stop();
        } else {
            bed_run();
        }
    }

    return bed_current_status.state;
}
