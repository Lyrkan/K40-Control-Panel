#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include "queues.h"
#include "Grbl/grbl_report.h"
#include "Grbl/grbl_state.h"

GrblReport::GrblReport() {
    state = GRBL_STATE_UNKNOWN;
    w_pos = {
        .is_set = false,
        .x = 0,
        .y = 0,
        .z = 0,
    };
    m_pos = {
        .is_set = false,
        .x = 0,
        .y = 0,
        .z = 0,
    };
    wco = {
        .is_set = false,
        .x = 0,
        .y = 0,
        .z = 0,
    };
    buffer_state = {
        .is_set = false,
        .planned_buffer_available_blocks = 0,
        .rx_buffer_available_bytes = 0,
    };
    feed_state = {
        .is_set = false,
        .rate = 0,
        .spindle_speed = 0,
    };
    line_number = 0;
    active_pins = 0;
}

void GrblReport::update(const GrblReport *report) {
    if (report->state > GRBL_STATE_INVALID) {
        state = report->state;
    }

    if (report->w_pos.is_set) {
        w_pos = report->w_pos;
    }

    if (report->m_pos.is_set) {
        m_pos = report->m_pos;
    }

    if (report->wco.is_set) {
        wco = report->wco;
    }

    if (report->buffer_state.is_set) {
        buffer_state = report->buffer_state;
    }

    if (report->feed_state.is_set) {
        feed_state = report->feed_state;
    }

    if (report->active_pins > GRBL_PIN_FLAG_UNKNOWN) {
        active_pins = report->active_pins;
    }

    if (report->line_number >= 0) {
        line_number = report->line_number;
    }

    // Update MPos or WPos using Work Coordinate Offset if available
    if (wco.is_set) {
        if (report->m_pos.is_set && !report->w_pos.is_set) {
            w_pos.is_set = true;
            w_pos.x = m_pos.x - wco.x;
            w_pos.y = m_pos.y - wco.y;
            w_pos.z = m_pos.z - wco.z;
        }

        if (report->w_pos.is_set && !report->m_pos.is_set) {
            m_pos.is_set = true;
            m_pos.x = w_pos.x + wco.x;
            m_pos.y = w_pos.y + wco.y;
            m_pos.z = w_pos.z + wco.z;
        }
    }

    log_d("Report updated:");
    log_d("  State: %s", grbl_state_to_string(state));
    log_d("  WPos: %.2f, %.2f, %.2f (is_set=%d)", w_pos.x, w_pos.y, w_pos.z, w_pos.is_set);
    log_d("  MPos: %.2f, %.2f, %.2f (is_set=%d)", m_pos.x, m_pos.y, m_pos.z, m_pos.is_set);
    log_d("  WCO: %.2f, %.2f, %.2f (is_set=%d)", wco.x, wco.y, wco.z, wco.is_set);
    log_d(
        "  Buffer state: %d, %d (is_set=%d)",
        buffer_state.planned_buffer_available_blocks,
        buffer_state.rx_buffer_available_bytes,
        buffer_state.is_set);
    log_d("  Feed state: %d, %d (is_set=%d)", feed_state.rate, feed_state.spindle_speed, feed_state.is_set);
    log_d("  Active pins: %d", active_pins);
    log_d("  Line number: %d", line_number);
}

static GrblReport grbl_last_report;
void grbl_update_last_report(const GrblReport *report) {
    grbl_last_report.update(report);
    xQueueOverwrite(grbl_report_update_queue, &grbl_last_report);
}
