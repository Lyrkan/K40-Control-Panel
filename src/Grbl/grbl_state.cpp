#include <string.h>

#include "Grbl/grbl_state.h"

GrblState grbl_state_from_string(char *state) {
    if (strcmp(state, "Idle") == 0) {
        return GRBL_STATE_IDLE;
    }

    if (strcmp(state, "Run") == 0) {
        return GRBL_STATE_RUN;
    }

    if (strcmp(state, "Alarm") == 0) {
        return GRBL_STATE_ALARM;
    }

    if (strcmp(state, "Check") == 0) {
        return GRBL_STATE_CHECK;
    }

    if (strcmp(state, "Home") == 0) {
        return GRBL_STATE_HOME;
    }

    if (strcmp(state, "Sleep") == 0) {
        return GRBL_STATE_SLEEP;
    }

    if ((strncmp(state, "Hold:", 5) == 0) && (state[5] != '\0')) {
        // - Hold:0 - Hold complete. Ready to resume.
        // - Hold:1 - Hold in-progress. Reset will throw an alarm.
        int hold_state = atoi(state + 5);
        switch (hold_state) {
        case 0:
            return GRBL_STATE_HOLD_0;
        case 1:
            return GRBL_STATE_HOLD_1;
        }
    }

    if ((strncmp(state, "Door:", 5) == 0) && (state[5] != '\0')) {
        // - Door:0 - Door closed. Ready to resume.
        // - Door:1 - Machine stopped. Door still ajar. Can't resume until closed.
        // - Door:2 - Door opened. Hold (or parking retract) in-progress. Reset will throw an alarm.
        // - Door:3 - Door closed and resuming. Restoring from park, if applicable. Reset will throw an alarm.
        int door_state = atoi(state + 5);
        switch (door_state) {
        case 0:
            return GRBL_STATE_DOOR_0;
        case 1:
            return GRBL_STATE_DOOR_1;
        case 2:
            return GRBL_STATE_DOOR_2;
        case 3:
            return GRBL_STATE_DOOR_3;
        }
    }

    return GRBL_STATE_INVALID;
}

const char *grbl_state_to_string(GrblState state) {
    switch (state) {
    case GRBL_STATE_IDLE:
        return "Idle";
    case GRBL_STATE_RUN:
        return "Run";
    case GRBL_STATE_HOLD_0:
        return "Hold: complete";
    case GRBL_STATE_HOLD_1:
        return "Hold: in-progress";
    case GRBL_STATE_ALARM:
        return "Alarm";
    case GRBL_STATE_DOOR_0:
        return "Door: closed";
    case GRBL_STATE_DOOR_1:
        return "Door: machine stopped";
    case GRBL_STATE_DOOR_2:
        return "Door: hold";
    case GRBL_STATE_DOOR_3:
        return "Door: resuming";
    case GRBL_STATE_CHECK:
        return "Check";
    case GRBL_STATE_HOME:
        return "Home";
    case GRBL_STATE_SLEEP:
        return "Sleep";
    default:
        return "Invalid/unknown Grbl state";
    }
}

const char *grbl_alarm_to_string(GrblAlarm alarm) {
    switch (alarm) {
    case GRBL_ALARM_HARD_LIMIT:
        return "Hard limit triggered";
    case GRBL_ALARM_MOTION_EXCEEDS_MACHINE_TRAVEL:
        return "G-code motion target exceeds machine travel";
    case GRBL_ALARM_RESET_WHILE_IN_MOTION:
        return "Reset while in motion, re-homing is highly recommended";
    case GRBL_ALARM_PROBE_FAIL_INITIAL_STATE:
        return "Probe fail: The probe is not in the expected initial state before starting probe cycle";
    case GRBL_ALARM_PROBE_FAIL_TRAVEL:
        return "Probe fail: Probe did not contact the workpiece within the programmed travel";
    case GRBL_ALARM_HOMING_FAIL_RESET:
        return "Homing fail: Reset during active homing cycle";
    case GRBL_ALARM_HOMING_FAIL_DOOR:
        return "Homing fail: Safety door was opened during active homing cycle";
    case GRBL_ALARM_HOMING_FAIL_LIMIT_CYCLE:
        return "Homing fail: Cycle failed to clear limit switch when pulling off, try increasing pull-off setting or "
               "check wiring";
    case GRBL_ALARM_HOMING_FAIL_SWITCH_NOT_FOUND:
        return "Homing fail: Could not find limit switch within search distance";
    default:
        return "Invalid/unknown Grbl alarm";
    }
}

const char *grbl_error_to_string(GrblError error) {
    switch (error) {
    case GRBL_ERROR_INVALID_GCODE_WORD:
        return "G-code words consist of a letter and a value. Letter was not found.";
    case GRBL_ERROR_NUMERIC_VALUE_FORMAT:
        return "Numeric value format is not valid or missing an expected value.";
    case GRBL_ERROR_UNRECOGNIZED_SYSTEM_COMMAND:
        return "Grbl '$' system command was not recognized or supported.";
    case GRBL_ERROR_POSITIVE_VALUE_EXPECTED:
        return "Negative value received for an expected positive value.";
    case GRBL_ERROR_HOMING_CYCLE_NOT_ENABLED:
        return "Homing cycle is not enabled via settings.";
    case GRBL_ERROR_MINIMUM_STEP_PULSE_TIME:
        return "Minimum step pulse time must be greater than 3usec.";
    case GRBL_ERROR_EEPROM_READ_FAILED:
        return "EEPROM read failed. Reset and restored to default values.";
    case GRBL_ERROR_IDLE_REQUIRED:
        return "Grbl '$' command cannot be used unless Grbl is IDLE. Ensures smooth operation during a job.";
    case GRBL_ERROR_GCODE_LOCKED_DURING_ALARM_OR_JOG:
        return "G-code locked out during alarm or jog state.";
    case GRBL_ERROR_SOFT_LIMIT_DISABLED_WITHOUT_HOMING:
        return "Soft limits cannot be enabled without homing also enabled.";
    case GRBL_ERROR_MAX_CHARACTERS_PER_LINE_EXCEEDED:
        return "Max characters per line exceeded. Line was not processed and executed.";
    case GRBL_ERROR_MAXIMUM_STEP_RATE_EXCEEDED:
        return "Grbl '$' setting value exceeds the maximum step rate supported.";
    case GRBL_ERROR_SAFETY_DOOR_OPENED_AND_DOOR_STATE_INITIATED:
        return "Safety door detected as opened and door state initiated.";
    case GRBL_ERROR_BUILD_INFO_EXCEEDED_EEPROM_LINE_LENGTH_LIMIT:
        return "Build info or startup line exceeded EEPROM line length limit.";
    case GRBL_ERROR_JOG_TARGET_EXCEEDS_MACHINE_TRAVEL:
        return "Jog target exceeds machine travel. Command ignored.";
    case GRBL_ERROR_INVALID_JOG_COMMAND:
        return "Jog command with no '=' or contains prohibited g-code.";
    case GRBL_ERROR_LASER_MODE_DISABLED:
        return "Laser mode disabled. Requires PWM output.";
    case GRBL_ERROR_INVALID_GCODE_COMMAND:
        return "Unsupported or invalid g-code command found in block.";
    case GRBL_ERROR_TOO_MANY_GCODE_COMMANDS_IN_BLOCK:
        return "More than one g-code command from same modal group found in block.";
    case GRBL_ERROR_FEED_RATE_UNDEFINED:
        return "Feed rate has not yet been set or is undefined.";
    case GRBL_ERROR_INTEGER_VALUE_REQUIRED:
        return "G-code command in block requires an integer value.";
    case GRBL_ERROR_MULTIPLE_COMMAND_WITH_XYZ_WORDS_IN_BLOCK:
        return "Two G-code commands that both require the use of the `XYZ` axis words were detected in the block.";
    case GRBL_ERROR_GCODE_WORD_REPEATED_IN_BLOCK:
        return "A G-code word was repeated in the block.";
    case GRBL_ERROR_GCODE_COMMAND_REQUIRES_XYZ_WORDS_IN_BLOCK:
        return "A G-code command implicitly or explicitly requires `XYZ` axis words in the block, but none were "
               "detected.";
    case GRBL_ERROR_LINE_NUMBER_VALUE_NOT_IN_RANGE:
        return "`N` line number value is not within the valid range of `1` - `9,999,999`.";
    case GRBL_ERROR_MISSING_P_OR_L_VALUE_WORDS:
        return "A G-code command was sent, but is missing some required `P` or `L` value words in the line.";
    case GRBL_ERROR_SIX_WORK_COORDINATE_SYSTEMS_SUPPORTED:
        return "Grbl supports six work coordinate systems `G54-G59`. `G59.1`, `G59.2`, and `G59.3` are not supported.";
    case GRBL_ERROR_G53_REQUIRES_G0_OR_G1:
        return "The `G53` G-code command requires either a `G0` seek or `G1` feed motion mode to be active. A "
               "different motion was active.";
    case GRBL_ERROR_UNUSED_AXIS_WORDS_IN_BLOCK:
        return "There are unused axis words in the block and `G80` motion mode cancel is active.";
    case GRBL_ERROR_G2_OR_G3_WITHOUT_XYZ_WORDS:
        return "A `G2` or `G3` arc was commanded but there are no `XYZ` axis words in the selected plane to trace the "
               "arc.";
    case GRBL_ERROR_MOTION_COMMAND_INVALID_TARGET:
        return "The motion command has an invalid target. `G2`, `G3`, and `G38.2` generates this error, if the arc is "
               "impossible to generate or if the probe target is the current position.";
    case GRBL_ERROR_G2_OR_G3_COMPUTATION_FAILED:
        return "A `G2` or `G3` arc, traced with the radius definition, had a mathematical error when computing the arc "
               "geometry. Try either breaking up the arc into semi-circles or quadrants, or redefine them with the arc "
               "offset definition.";
    case GRBL_ERROR_G2_OR_G3_MISSING_IJK_OFFSET:
        return "A `G2` or `G3` arc, traced with the offset definition, is missing the `IJK` offset word in the "
               "selected plane to trace the arc.";
    case GRBL_ERROR_UNUSED_LEFTOVER_GCODE_WORDS:
        return "There are unused, leftover G-code words that aren't used by any command in the block.";
    case GRBL_ERROR_DYNAMIC_TOOL_LENGTH_OFFSET_INVALID_AXIS:
        return "The `G43.1` dynamic tool length offset command cannot apply an offset to an axis other than its "
               "configured axis. The Grbl default axis is the Z-axis.";
    case GRBL_ERROR_TOOL_NUMBER_GREATER_THAN_MAX_VALUE:
        return "Tool number greater than max supported value.";
    default:
        return "Invalid/unknown Grbl error";
    }
}
