#ifndef UI_UTILS_H
#define UI_UTILS_H

#include <lvgl.h>

#define LV_STATE_CARD_ERROR LV_STATE_USER_1

lv_obj_t *ui_utils_create_screen_panel(lv_obj_t *parent);

lv_obj_t *ui_utils_create_card(
    lv_obj_t *parent, const char *title, const void *icon = NULL, lv_color_t icon_color = lv_color_hex(0xFAFAFA));

lv_obj_t *ui_utils_create_small_button(lv_obj_t *parent, const char *label, lv_coord_t width = LV_SIZE_CONTENT);

void ui_utils_toggle_state(lv_obj_t *obj, lv_state_t state, bool enable);

#endif
