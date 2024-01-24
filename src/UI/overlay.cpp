#include <Arduino.h>
#include <lvgl.h>

#include "UI/overlay.h"
#include "queues.h"

static lv_obj_t *ui_overlay_flash_container;
static QueueHandle_t ui_overlay_pending_flash_queue = xQueueCreate(FLASH_MESSAGE_MAX_DISPLAYED, sizeof(FlashMessage *));
static FlashMessageLinkedList ui_overlay_flash_list = {
    .first = NULL,
    .last = NULL,
};

static void ui_overlay_display_flash_message(FlashMessage *flash) {
    // Add the new flash message to the list:
    // - Change the "next" pointer of the last element (if it exists)
    // - Set the "previous" pointer of the new element to the previous one (if it exists)
    // - Set the first element of the list if there is currently none
    // - Update the last element of the list
    FlashMessage *previous = ui_overlay_flash_list.last;
    if (previous != NULL) {
        previous->next = flash;
        flash->previous = previous;
    }

    if (ui_overlay_flash_list.first == NULL) {
        ui_overlay_flash_list.first = flash;
    }

    ui_overlay_flash_list.last = flash;

    // Create the LVGL objects representing the flash message
    if (flash->ui_element != NULL) {
        // Should never happen...
        lv_obj_clean(flash->ui_element);
    }

    flash->ui_element = lv_obj_create(ui_overlay_flash_container);
    lv_obj_set_width(flash->ui_element, LV_SIZE_CONTENT);
    lv_obj_set_height(flash->ui_element, LV_SIZE_CONTENT);
    lv_obj_set_style_border_width(flash->ui_element, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_ver(flash->ui_element, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_hor(flash->ui_element, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(flash->ui_element, 200, LV_PART_MAIN | LV_STATE_DEFAULT);

    switch (flash->level) {
    case FLASH_LEVEL_INFO:
        lv_obj_set_style_bg_color(flash->ui_element, lv_color_hex(0x007bff), LV_PART_MAIN | LV_STATE_DEFAULT);
        break;
    case FLASH_LEVEL_WARNING:
        lv_obj_set_style_bg_color(flash->ui_element, lv_color_hex(0xffc107), LV_PART_MAIN | LV_STATE_DEFAULT);
        break;
    case FLASH_LEVEL_DANGER:
        lv_obj_set_style_bg_color(flash->ui_element, lv_color_hex(0xdc3545), LV_PART_MAIN | LV_STATE_DEFAULT);
        break;
    default:
        // Should never happen...
        lv_obj_set_style_bg_color(flash->ui_element, lv_color_black(), LV_PART_MAIN | LV_STATE_DEFAULT);
        break;
    }

    lv_obj_t *flash_label = lv_label_create(flash->ui_element);
    lv_label_set_text(flash_label, flash->message);
    lv_obj_set_width(flash_label, LV_SIZE_CONTENT);
    lv_obj_set_height(flash_label, LV_SIZE_CONTENT);
    lv_obj_set_style_max_width(flash_label, lv_obj_get_width(lv_layer_top()) * 0.8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_label_set_long_mode(flash_label, LV_LABEL_LONG_SCROLL);
    lv_obj_set_style_text_color(flash_label, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(flash_label, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(flash_label, lv_color_white(), LV_PART_MAIN | LV_STATE_DEFAULT);
}

static void ui_overlay_dispose_flash_message(FlashMessage *flash) {
    // Remove the flash message from the list:
    // - Update the "next" pointer of the previous element if there is one
    // - Update the "previous" pointer of the next element if there is one
    // - Update the first element of the list if it matches the removed flash message
    // - Update the last element of the list if it matches the removed flash message
    if (flash->previous != NULL) {
        flash->previous->next = flash->next;
    }

    if (flash->next != NULL) {
        flash->next->previous = flash->previous;
    }

    if (ui_overlay_flash_list.first == flash) {
        ui_overlay_flash_list.first = flash->next;
    }

    if (ui_overlay_flash_list.last == flash) {
        ui_overlay_flash_list.last = flash->previous;
    }

    // Cleanup the memory (LVGL objects, message and struct)
    if (flash->ui_element != NULL) {
        lv_obj_del(flash->ui_element);
    }

    free(flash->message);
    free(flash);
}

void ui_overlay_init() {
    ui_overlay_flash_container = lv_obj_create(lv_layer_top());
    lv_obj_set_align(ui_overlay_flash_container, LV_ALIGN_TOP_RIGHT);
    lv_obj_set_x(ui_overlay_flash_container, -10);
    lv_obj_set_y(ui_overlay_flash_container, 60);
    lv_obj_set_height(ui_overlay_flash_container, LV_SIZE_CONTENT);
    lv_obj_set_width(ui_overlay_flash_container, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(ui_overlay_flash_container, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(ui_overlay_flash_container, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_END, LV_FLEX_ALIGN_START);
    lv_obj_set_style_border_width(ui_overlay_flash_container, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_overlay_flash_container, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(ui_overlay_flash_container, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(ui_overlay_flash_container, LV_OBJ_FLAG_CLICKABLE);
}

void ui_overlay_update() {

    // Display new messages if needed
    FlashMessage *new_message = NULL;
    while (xQueueReceive(ui_overlay_pending_flash_queue, &(new_message), 0) != pdFALSE) {
        ui_overlay_display_flash_message(new_message);
        log_d("Flash message displayed: %s (level: %d)", new_message->message, new_message->level);
    }

    // Remove expired / overflowing messages
    unsigned int flash_messages_count = 0;
    unsigned long current_time = millis();
    FlashMessage *flash_message = ui_overlay_flash_list.last;
    while (flash_message != NULL) {
        FlashMessage *previous_message = flash_message->previous;
        flash_messages_count++;
        if (((flash_message->created_at + FLASH_MESSAGE_DURATION_MS) <= current_time) ||
            (flash_messages_count > FLASH_MESSAGE_MAX_DISPLAYED)) {
            log_d(
                "Flash message about to be disposed: %s (level: %d, created_at: %ld)",
                flash_message->message,
                flash_message->level,
                flash_message->created_at);
            ui_overlay_dispose_flash_message(flash_message);
        }

        flash_message = previous_message;
    }
}

void ui_overlay_add_flash_message(FlashLevel level, const char *message) {
    log_d("Flash message scheduled for display: %s (level: %d)", message, level);

    // Schedule the flash message to be displayed on the screen
    FlashMessage *flash = (FlashMessage *)malloc(sizeof(FlashMessage));
    flash->level = level;
    flash->message = strndup(message, FLASH_MESSAGE_MAX_LENGTH);
    flash->created_at = millis();
    flash->ui_element = NULL;
    flash->previous = NULL;
    flash->next = NULL;
    xQueueSendToBack(ui_overlay_pending_flash_queue, (void *)&flash, pdMS_TO_TICKS(100));
}
