#include <lvgl.h>

#include "UI/menu.h"
#include "UI/images.h"
#include "UI/screens/splashscreen.h"
#include "UI/screens/status.h"

lv_obj_t *ui_splashscreen_screen;

static lv_obj_t *ui_splashscreen_logo;
static lv_obj_t *ui_splashscreen_title;
static lv_obj_t *ui_splashscreen_build_date;

static void ui_splashscreen_event_handler(lv_event_t *e) {
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);
    switch (event_code) {
    case LV_EVENT_SCREEN_LOADED:
        lv_scr_load_anim(ui_status_screen, LV_SCR_LOAD_ANIM_FADE_ON, 500, 3000, false);
        break;
    case LV_EVENT_SCREEN_UNLOADED:
        ui_menu_init();
        break;
    }
}

void ui_splashscreen_init() {
    ui_splashscreen_screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(ui_splashscreen_screen, lv_color_hex(0x442E76), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_splashscreen_screen, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(ui_splashscreen_screen, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_event_cb(ui_splashscreen_screen, ui_splashscreen_event_handler, LV_EVENT_ALL, NULL);

    ui_splashscreen_logo = lv_img_create(ui_splashscreen_screen);
    lv_img_set_src(ui_splashscreen_logo, &image_splashscreen);
    lv_obj_set_width(ui_splashscreen_logo, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_splashscreen_logo, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_splashscreen_logo, 0);
    lv_obj_set_y(ui_splashscreen_logo, 115);
    lv_obj_set_align(ui_splashscreen_logo, LV_ALIGN_TOP_LEFT);

    ui_splashscreen_build_date = lv_label_create(ui_splashscreen_screen);
    lv_obj_set_width(ui_splashscreen_build_date, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_splashscreen_build_date, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_splashscreen_build_date, 50);
    lv_obj_set_y(ui_splashscreen_build_date, 228);
    lv_obj_set_align(ui_splashscreen_logo, LV_ALIGN_TOP_LEFT);
    lv_obj_set_style_text_color(ui_splashscreen_build_date, lv_color_white(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_label_set_text(ui_splashscreen_build_date, "Build: " __DATE__ " " __TIME__ " (" GIT_CURRENT_REF ")");
}
