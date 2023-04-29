#include "UI/menu.h"
#include "UI/overlay.h"
#include "UI/ui.h"
#include "UI/screens/bed.h"
#include "UI/screens/controls.h"
#include "UI/screens/settings.h"
#include "UI/screens/splashscreen.h"
#include "UI/screens/status.h"

void ui_init() {
    // Set theme
    lv_disp_t *default_display = lv_disp_get_default();
    lv_theme_t *theme = lv_theme_default_init(
        default_display,
        lv_palette_main(LV_PALETTE_BLUE),
        lv_palette_main(LV_PALETTE_RED),
        false,
        LV_FONT_DEFAULT);
    lv_disp_set_theme(default_display, theme);

    // Initialize alerts overlay
    ui_overlay_init();

    // Initialize screens
    ui_splashscreen_init();
    ui_status_init();
    ui_controls_init();
    ui_bed_init();
    ui_settings_init();

    // Display splashscreen
    lv_disp_load_scr(ui_splashscreen_screen);
}

void ui_update() {
    ui_overlay_update();
    ui_menu_indicators_update();
    ui_status_update();
    ui_controls_update();
    ui_bed_update();
    ui_settings_update();
}
