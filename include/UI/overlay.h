#ifndef UI_OVERLAY_H
#define UI_OVERLAY_H

#include <lvgl.h>

#define FLASH_MESSAGE_DURATION_MS 5000
#define FLASH_MESSAGE_MAX_DISPLAYED 5
#define FLASH_MESSAGE_MAX_LENGTH 255

typedef enum {
    FLASH_LEVEL_SUCCESS,
    FLASH_LEVEL_INFO,
    FLASH_LEVEL_WARNING,
    FLASH_LEVEL_DANGER,
} FlashLevel;

typedef struct FlashMessage {
    FlashLevel level;
    char *message;
    lv_obj_t *ui_element;
    unsigned long created_at;
    struct FlashMessage *previous;
    struct FlashMessage *next;
} FlashMessage;

typedef struct {
    FlashMessage *first;
    FlashMessage *last;
} FlashMessageLinkedList;

void ui_overlay_init();
void ui_overlay_update();
void ui_overlay_add_flash_message(FlashLevel level, const char *message);

#endif
