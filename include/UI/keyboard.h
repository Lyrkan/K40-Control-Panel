#ifndef UI_KEYBOARD_H
#define UI_KEYBOARD_H

#include <lvgl.h>

// clang-format off
const char *KEYBOARD_NUMPAD_MAP[] = {
    "1", "2", "3", LV_SYMBOL_BACKSPACE, "\n",
    "4", "5", "6", LV_SYMBOL_LEFT, LV_SYMBOL_RIGHT, "\n",
    "7", "8", "9", "0", ".", NULL
};

const lv_btnmatrix_ctrl_t KEYBOARD_NUMPAD_CTRL[] = {
    1, 1, 1, 2,
    1, 1, 1, 1, 1,
    1, 1, 1, 1, 1
};
// clang-format on

#endif
