#include <lvgl.h>
#include "UI/utils.h"

lv_obj_t *ui_utils_create_screen_panel(lv_obj_t *parent) {
    lv_obj_t *panel = lv_obj_create(parent);
    lv_obj_set_width(panel, 480);
    lv_obj_set_height(panel, 275);
    lv_obj_set_x(panel, 0);
    lv_obj_set_y(panel, 0);
    lv_obj_set_style_pad_all(panel, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_align(panel, LV_ALIGN_BOTTOM_LEFT);
    lv_obj_set_style_bg_opa(panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(panel, LV_OBJ_FLAG_SCROLLABLE);

    return panel;
}

lv_obj_t *ui_utils_create_card(lv_obj_t *parent, const char *title, const void *icon, lv_color_t icon_color) {
    lv_obj_t *card = lv_obj_create(parent);
    lv_obj_set_style_pad_all(card, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(card, lv_color_white(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(card, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_color(card, lv_color_hex(0x333333), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(card, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_ofs_x(card, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_ofs_y(card, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_opa(card, 75, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(card, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(card, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *label = lv_label_create(card);
    lv_obj_set_width(label, LV_SIZE_CONTENT);
    lv_obj_set_height(label, LV_SIZE_CONTENT);
    lv_obj_set_align(label, LV_ALIGN_TOP_LEFT);
    lv_obj_set_style_text_font(label, &font_default_16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(label, lv_color_hex(0xA0A0A0), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_label_set_text(label, title);

    if (icon != NULL) {
        lv_obj_t *image = lv_img_create(card);
        lv_img_set_src(image, icon);
        lv_obj_set_width(image, LV_SIZE_CONTENT);
        lv_obj_set_height(image, LV_SIZE_CONTENT);
        lv_obj_set_align(image, LV_ALIGN_TOP_RIGHT);
        lv_obj_set_pos(image, 5, -8);
        lv_obj_set_style_text_color(image, icon_color, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(image, &font_icons_36, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_align(image, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
    }

    return card;
}

lv_obj_t *ui_utils_create_small_button(lv_obj_t *parent, const char *label, lv_coord_t width) {
    lv_obj_t *button = lv_btn_create(parent);
    lv_obj_set_size(button, width, 25);
    lv_obj_set_style_radius(button, 6, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *button_label = lv_label_create(button);
    lv_obj_set_width(button_label, LV_SIZE_CONTENT);
    lv_obj_set_height(button_label, LV_SIZE_CONTENT);
    lv_obj_set_align(button_label, LV_ALIGN_CENTER);
    lv_label_set_text(button_label, label);

    return button;
}
