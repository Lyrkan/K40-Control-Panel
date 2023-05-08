#!/bin/bash

FONT_SIZES=(12 14)
DEFAULT_FONT="Roboto-Regular.ttf"
RANGE_DEFAULT="0x20-0x7F,0xB0,0x2022"

# Font Awesome glyphs:
#
# * 0xF11C = fa-keyboard
# * 0xF3BE = fa-turn-down
# * 0xF053 = fa-chevron-left
# * 0xF054 = fa-chevron-right
# * 0xF55A = fa-delete-left
# * 0xF00C = fa-check
# * 0xF00D = fa-xmark
# * 0xF071 = fa-triangle-exclamation
# * 0xF013 = fa-gear
# * 0xF002 = fa-magnifying-glass
# * 0xF204 = fa-toggle-off
# * 0xE4AF = fa-arrow-down-up-across-line
#
RANGE_FA_REGULAR="0xF11C"
RANGE_FA_SOLID="0xF3BE,0xF053,0xF054,0xF55A,0xF00C,0xF00D,0xF071,0xF068,0xF067,0xF013,0xF002,0xF204,0xE4AF"

cd "$(dirname "${BASH_SOURCE[0]}")"

for FONT_SIZE in "${FONT_SIZES[@]}"
do
    lv_font_conv --lcd --no-compress --bpp 4 --size ${FONT_SIZE} --font "./fonts/${DEFAULT_FONT}" -r "${RANGE_DEFAULT}" --font ./fonts/fa-regular-400.ttf -r "${RANGE_FA_REGULAR}" --font ./fonts/fa-solid-900.ttf -r "${RANGE_FA_SOLID}" --format lvgl --no-kern --lv-include "lvgl.h" -o "../src/UI/fonts/font_default_${FONT_SIZE}.c"
done



# Font Awesome glyphs:
#
# * 0xF06D = fa-fire
# * 0xF2DC = fa-snowflake
# * 0xF0E7 = fa-bolt
# * 0xF06E = fa-eye
ICONS_SIZES=(40)
RANGE_ICONS_FA_SOLID="0xF06D,0xF2DC,0xF0E7,0xF06E"

for FONT_SIZE in "${ICONS_SIZES[@]}"
do
    lv_font_conv --lcd --no-compress --bpp 4 --size ${FONT_SIZE} --font ./fonts/fa-solid-900.ttf -r "${RANGE_ICONS_FA_SOLID}" --format lvgl --no-kern --lv-include "lvgl.h" -o "../src/UI/fonts/font_icons_${FONT_SIZE}.c"
done
