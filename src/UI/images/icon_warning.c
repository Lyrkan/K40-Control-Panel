#ifdef __has_include
    #if __has_include("lvgl.h")
        #ifndef LV_LVGL_H_INCLUDE_SIMPLE
            #define LV_LVGL_H_INCLUDE_SIMPLE
        #endif
    #endif
#endif

#if defined(LV_LVGL_H_INCLUDE_SIMPLE)
    #include "lvgl.h"
#else
    #include "lvgl/lvgl.h"
#endif


#ifndef LV_ATTRIBUTE_MEM_ALIGN
#define LV_ATTRIBUTE_MEM_ALIGN
#endif

#ifndef LV_ATTRIBUTE_IMG_ICON_WARNING
#define LV_ATTRIBUTE_IMG_ICON_WARNING
#endif

const LV_ATTRIBUTE_MEM_ALIGN LV_ATTRIBUTE_LARGE_CONST LV_ATTRIBUTE_IMG_ICON_WARNING uint8_t icon_warning_map[] = {
#if LV_COLOR_DEPTH == 1 || LV_COLOR_DEPTH == 8
  /*Pixel format: Alpha 8 bit, Red: 3 bit, Green: 3 bit, Blue: 2 bit*/
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe9, 0x0b, 0xe9, 0x35, 0xe9, 0x0b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe9, 0x08, 0xe9, 0xb8, 0xe9, 0xff, 0xe9, 0xb8, 0xe9, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe9, 0x6f, 0xe9, 0xff, 0xe9, 0xff, 0xe9, 0xff, 0xe9, 0x6f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe9, 0x19, 0xe9, 0xe5, 0xe9, 0xff, 0xe9, 0xff, 0xe9, 0xff, 0xe9, 0xe5, 0xe9, 0x19, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe9, 0x91, 0xe9, 0xff, 0xe9, 0xff, 0xe9, 0xff, 0xe9, 0xff, 0xe9, 0xff, 0xe9, 0x91, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe9, 0x30, 0xe9, 0xf5, 0xe9, 0xff, 0xe9, 0xff, 0xf2, 0xff, 0xe9, 0xff, 0xe9, 0xff, 0xe9, 0xf5, 0xe9, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe9, 0xb2, 0xe9, 0xff, 0xe5, 0xff, 0xf7, 0xff, 0xff, 0xff, 0xf7, 0xff, 0xe5, 0xff, 0xe9, 0xff, 0xe9, 0xb2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe9, 0x54, 0xe9, 0xff, 0xe9, 0xff, 0xe5, 0xff, 0xfb, 0xff, 0xff, 0xff, 0xfb, 0xff, 0xe5, 0xff, 0xe9, 0xff, 0xe9, 0xff, 0xe9, 0x54, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe9, 0x0a, 0xe9, 0xd2, 0xe9, 0xff, 0xe9, 0xff, 0xe5, 0xff, 0xfb, 0xff, 0xff, 0xff, 0xfb, 0xff, 0xe5, 0xff, 0xe9, 0xff, 0xe9, 0xff, 0xe9, 0xd2, 0xe9, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe9, 0x76, 0xe9, 0xff, 0xe9, 0xff, 0xe9, 0xff, 0xe5, 0xff, 0xfb, 0xff, 0xff, 0xff, 0xfb, 0xff, 0xe5, 0xff, 0xe9, 0xff, 0xe9, 0xff, 0xe9, 0xff, 0xe9, 0x76, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe9, 0x22, 0xe9, 0xeb, 0xe9, 0xff, 0xe9, 0xff, 0xe9, 0xff, 0xe5, 0xff, 0xfb, 0xff, 0xff, 0xff, 0xfb, 0xff, 0xe5, 0xff, 0xe9, 0xff, 0xe9, 0xff, 0xe9, 0xff, 0xe9, 0xeb, 0xe9, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe9, 0x9c, 0xe9, 0xff, 0xe9, 0xff, 0xe9, 0xff, 0xe9, 0xff, 0xe5, 0xff, 0xf6, 0xff, 0xff, 0xff, 0xf6, 0xff, 0xe5, 0xff, 0xe9, 0xff, 0xe9, 0xff, 0xe9, 0xff, 0xe9, 0xff, 0xe9, 0x9b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0xe9, 0x3b, 0xe9, 0xfc, 0xe9, 0xff, 0xe9, 0xff, 0xe9, 0xff, 0xe9, 0xff, 0xe9, 0xff, 0xe9, 0xff, 0xe9, 0xff, 0xe9, 0xff, 0xe9, 0xff, 0xe9, 0xff, 0xe9, 0xff, 0xe9, 0xff, 0xe9, 0xff, 0xe9, 0xfc, 0xe9, 0x3b, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0xf2, 0x02, 0xe9, 0xbe, 0xe9, 0xff, 0xe9, 0xff, 0xe9, 0xff, 0xe9, 0xff, 0xe9, 0xff, 0xe9, 0xff, 0xf2, 0xff, 0xfb, 0xff, 0xf2, 0xff, 0xe9, 0xff, 0xe9, 0xff, 0xe9, 0xff, 0xe9, 0xff, 0xe9, 0xff, 0xe9, 0xff, 0xe9, 0xbe, 0xf2, 0x02, 0x00, 0x00,
  0x00, 0x00, 0xe9, 0x5c, 0xe9, 0xff, 0xe9, 0xff, 0xe9, 0xff, 0xe9, 0xff, 0xe9, 0xff, 0xe9, 0xff, 0xe5, 0xff, 0xfb, 0xff, 0xff, 0xff, 0xfb, 0xff, 0xe5, 0xff, 0xe9, 0xff, 0xe9, 0xff, 0xe9, 0xff, 0xe9, 0xff, 0xe9, 0xff, 0xe9, 0xff, 0xe9, 0x5c, 0x00, 0x00,
  0xe9, 0x08, 0xe9, 0xd1, 0xe9, 0xff, 0xe9, 0xff, 0xe9, 0xff, 0xe9, 0xff, 0xe9, 0xff, 0xe9, 0xff, 0xe9, 0xff, 0xee, 0xff, 0xf7, 0xff, 0xee, 0xff, 0xe9, 0xff, 0xe9, 0xff, 0xe9, 0xff, 0xe9, 0xff, 0xe9, 0xff, 0xe9, 0xff, 0xe9, 0xff, 0xe9, 0xd1, 0xe9, 0x08,
  0xe9, 0x09, 0xe9, 0xd3, 0xe9, 0xff, 0xe9, 0xff, 0xe9, 0xff, 0xe9, 0xff, 0xe9, 0xff, 0xe9, 0xff, 0xe9, 0xff, 0xe9, 0xff, 0xe5, 0xff, 0xe9, 0xff, 0xe9, 0xff, 0xe9, 0xff, 0xe9, 0xff, 0xe9, 0xff, 0xe9, 0xff, 0xe9, 0xff, 0xe9, 0xff, 0xe9, 0xd3, 0xe9, 0x09,
  0x00, 0x00, 0xe9, 0x35, 0xe9, 0x9b, 0xe9, 0xa2, 0xe9, 0xa1, 0xe9, 0xa1, 0xe9, 0xa1, 0xe9, 0xa1, 0xe9, 0xa1, 0xe9, 0xa1, 0xe9, 0xa1, 0xe9, 0xa1, 0xe9, 0xa1, 0xe9, 0xa1, 0xe9, 0xa1, 0xe9, 0xa1, 0xe9, 0xa1, 0xe9, 0xa2, 0xe9, 0x9b, 0xe9, 0x35, 0x00, 0x00,
#endif
#if LV_COLOR_DEPTH == 16 && LV_COLOR_16_SWAP == 0
  /*Pixel format: Alpha 8 bit, Red: 5 bit, Green: 6 bit, Blue: 5 bit*/
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x49, 0xea, 0x0b, 0x0a, 0xf2, 0x35, 0x49, 0xea, 0x0b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0xe2, 0x08, 0x0a, 0xf2, 0xb8, 0x0a, 0xf2, 0xff, 0x0a, 0xf2, 0xb8, 0x08, 0xe2, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0xf2, 0x6f, 0x0a, 0xf2, 0xff, 0x0a, 0xf2, 0xff, 0x0a, 0xf2, 0xff, 0x0a, 0xf2, 0x6f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe9, 0xe9, 0x19, 0x0a, 0xf2, 0xe5, 0x0a, 0xf2, 0xff, 0x0a, 0xf2, 0xff, 0x0a, 0xf2, 0xff, 0x0a, 0xf2, 0xe5, 0xe9, 0xe9, 0x19, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0xf2, 0x91, 0x0a, 0xf2, 0xff, 0x09, 0xf2, 0xff, 0xa8, 0xf1, 0xff, 0x09, 0xf2, 0xff, 0x0a, 0xf2, 0xff, 0x0a, 0xf2, 0x91, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0xf2, 0x30, 0x0a, 0xf2, 0xf5, 0x09, 0xf2, 0xff, 0x8b, 0xf2, 0xff, 0x10, 0xf4, 0xff, 0x8b, 0xf2, 0xff, 0x09, 0xf2, 0xff, 0x0a, 0xf2, 0xf5, 0x09, 0xf2, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0xf2, 0xb2, 0x0a, 0xf2, 0xff, 0x68, 0xf1, 0xff, 0x75, 0xfd, 0xff, 0xfe, 0xff, 0xff, 0x75, 0xfd, 0xff, 0x68, 0xf1, 0xff, 0x0a, 0xf2, 0xff, 0x0a, 0xf2, 0xb2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0xf2, 0x54, 0x0a, 0xf2, 0xff, 0x0a, 0xf2, 0xff, 0x48, 0xf1, 0xff, 0xf7, 0xfd, 0xff, 0xff, 0xff, 0xff, 0xf7, 0xfd, 0xff, 0x48, 0xf1, 0xff, 0x0a, 0xf2, 0xff, 0x0a, 0xf2, 0xff, 0x0a, 0xf2, 0x54, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6a, 0xea, 0x0a, 0x0a, 0xf2, 0xd2, 0x0a, 0xf2, 0xff, 0x0a, 0xf2, 0xff, 0x48, 0xf1, 0xff, 0xf7, 0xfd, 0xff, 0xff, 0xff, 0xff, 0xf7, 0xfd, 0xff, 0x48, 0xf1, 0xff, 0x0a, 0xf2, 0xff, 0x0a, 0xf2, 0xff, 0x0a, 0xf2, 0xd2, 0x6a, 0xea, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0xf2, 0x76, 0x0a, 0xf2, 0xff, 0x0a, 0xf2, 0xff, 0x0a, 0xf2, 0xff, 0x48, 0xf1, 0xff, 0xf7, 0xfd, 0xff, 0xff, 0xff, 0xff, 0xf7, 0xfd, 0xff, 0x48, 0xf1, 0xff, 0x0a, 0xf2, 0xff, 0x0a, 0xf2, 0xff, 0x0a, 0xf2, 0xff, 0x0a, 0xf2, 0x76, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x29, 0xf2, 0x22, 0x0a, 0xf2, 0xeb, 0x0a, 0xf2, 0xff, 0x0a, 0xf2, 0xff, 0x0a, 0xf2, 0xff, 0x47, 0xf1, 0xff, 0xf7, 0xfd, 0xff, 0xff, 0xff, 0xff, 0xf7, 0xfd, 0xff, 0x47, 0xf1, 0xff, 0x0a, 0xf2, 0xff, 0x0a, 0xf2, 0xff, 0x0a, 0xf2, 0xff, 0x0a, 0xf2, 0xeb, 0x29, 0xf2, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0xf2, 0x9c, 0x0a, 0xf2, 0xff, 0x0a, 0xf2, 0xff, 0x0a, 0xf2, 0xff, 0x0a, 0xf2, 0xff, 0x88, 0xf1, 0xff, 0x14, 0xfd, 0xff, 0x9d, 0xff, 0xff, 0x14, 0xfd, 0xff, 0x88, 0xf1, 0xff, 0x0a, 0xf2, 0xff, 0x0a, 0xf2, 0xff, 0x0a, 0xf2, 0xff, 0x0a, 0xf2, 0xff, 0x0a, 0xf2, 0x9b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0xf2, 0x3b, 0x0a, 0xf2, 0xfc, 0x0a, 0xf2, 0xff, 0x0a, 0xf2, 0xff, 0x0a, 0xf2, 0xff, 0x0a, 0xf2, 0xff, 0x0a, 0xf2, 0xff, 0xc9, 0xf1, 0xff, 0x4a, 0xf2, 0xff, 0xc9, 0xf1, 0xff, 0x0a, 0xf2, 0xff, 0x0a, 0xf2, 0xff, 0x0a, 0xf2, 0xff, 0x0a, 0xf2, 0xff, 0x0a, 0xf2, 0xff, 0x0a, 0xf2, 0xfc, 0x0a, 0xf2, 0x3b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x10, 0xfc, 0x02, 0x0a, 0xf2, 0xbe, 0x0a, 0xf2, 0xff, 0x0a, 0xf2, 0xff, 0x0a, 0xf2, 0xff, 0x0a, 0xf2, 0xff, 0x0a, 0xf2, 0xff, 0xc9, 0xf1, 0xff, 0xaf, 0xf3, 0xff, 0xf7, 0xfd, 0xff, 0xaf, 0xf3, 0xff, 0xc9, 0xf1, 0xff, 0x0a, 0xf2, 0xff, 0x0a, 0xf2, 0xff, 0x0a, 0xf2, 0xff, 0x0a, 0xf2, 0xff, 0x0a, 0xf2, 0xff, 0x0a, 0xf2, 0xbe, 0x10, 0xfc, 0x02, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x09, 0xf2, 0x5c, 0x0a, 0xf2, 0xff, 0x0a, 0xf2, 0xff, 0x0a, 0xf2, 0xff, 0x0a, 0xf2, 0xff, 0x0a, 0xf2, 0xff, 0x0a, 0xf2, 0xff, 0x48, 0xf1, 0xff, 0xb6, 0xfd, 0xff, 0xff, 0xff, 0xff, 0xb6, 0xfd, 0xff, 0x48, 0xf1, 0xff, 0x0a, 0xf2, 0xff, 0x0a, 0xf2, 0xff, 0x0a, 0xf2, 0xff, 0x0a, 0xf2, 0xff, 0x0a, 0xf2, 0xff, 0x0a, 0xf2, 0xff, 0x09, 0xf2, 0x5c, 0x00, 0x00, 0x00,
  0x08, 0xe2, 0x08, 0x0a, 0xf2, 0xd1, 0x0a, 0xf2, 0xff, 0x0a, 0xf2, 0xff, 0x0a, 0xf2, 0xff, 0x0a, 0xf2, 0xff, 0x0a, 0xf2, 0xff, 0x0a, 0xf2, 0xff, 0xe9, 0xf1, 0xff, 0x4d, 0xf3, 0xff, 0x75, 0xfd, 0xff, 0x4d, 0xf3, 0xff, 0xe9, 0xf1, 0xff, 0x0a, 0xf2, 0xff, 0x0a, 0xf2, 0xff, 0x0a, 0xf2, 0xff, 0x0a, 0xf2, 0xff, 0x0a, 0xf2, 0xff, 0x0a, 0xf2, 0xff, 0x0a, 0xf2, 0xd1, 0x08, 0xe2, 0x08,
  0xcb, 0xe1, 0x09, 0x0a, 0xf2, 0xd3, 0x0a, 0xf2, 0xff, 0x0a, 0xf2, 0xff, 0x0a, 0xf2, 0xff, 0x0a, 0xf2, 0xff, 0x0a, 0xf2, 0xff, 0x0a, 0xf2, 0xff, 0x0a, 0xf2, 0xff, 0xe9, 0xf1, 0xff, 0x88, 0xf1, 0xff, 0xe9, 0xf1, 0xff, 0x0a, 0xf2, 0xff, 0x0a, 0xf2, 0xff, 0x0a, 0xf2, 0xff, 0x0a, 0xf2, 0xff, 0x0a, 0xf2, 0xff, 0x0a, 0xf2, 0xff, 0x0a, 0xf2, 0xff, 0x0a, 0xf2, 0xd3, 0xcb, 0xe1, 0x09,
  0x00, 0x00, 0x00, 0x0a, 0xf2, 0x35, 0x0a, 0xf2, 0x9b, 0x0a, 0xf2, 0xa2, 0x0a, 0xf2, 0xa1, 0x0a, 0xf2, 0xa1, 0x0a, 0xf2, 0xa1, 0x0a, 0xf2, 0xa1, 0x0a, 0xf2, 0xa1, 0x0a, 0xf2, 0xa1, 0x0a, 0xf2, 0xa1, 0x0a, 0xf2, 0xa1, 0x0a, 0xf2, 0xa1, 0x0a, 0xf2, 0xa1, 0x0a, 0xf2, 0xa1, 0x0a, 0xf2, 0xa1, 0x0a, 0xf2, 0xa1, 0x0a, 0xf2, 0xa2, 0x0a, 0xf2, 0x9b, 0x0a, 0xf2, 0x35, 0x00, 0x00, 0x00,
#endif
#if LV_COLOR_DEPTH == 16 && LV_COLOR_16_SWAP != 0
  /*Pixel format: Alpha 8 bit, Red: 5 bit, Green: 6 bit, Blue: 5 bit  BUT the 2  color bytes are swapped*/
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xea, 0x49, 0x0b, 0xf2, 0x0a, 0x35, 0xea, 0x49, 0x0b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe2, 0x08, 0x08, 0xf2, 0x0a, 0xb8, 0xf2, 0x0a, 0xff, 0xf2, 0x0a, 0xb8, 0xe2, 0x08, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf2, 0x0a, 0x6f, 0xf2, 0x0a, 0xff, 0xf2, 0x0a, 0xff, 0xf2, 0x0a, 0xff, 0xf2, 0x0a, 0x6f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe9, 0xe9, 0x19, 0xf2, 0x0a, 0xe5, 0xf2, 0x0a, 0xff, 0xf2, 0x0a, 0xff, 0xf2, 0x0a, 0xff, 0xf2, 0x0a, 0xe5, 0xe9, 0xe9, 0x19, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf2, 0x0a, 0x91, 0xf2, 0x0a, 0xff, 0xf2, 0x09, 0xff, 0xf1, 0xa8, 0xff, 0xf2, 0x09, 0xff, 0xf2, 0x0a, 0xff, 0xf2, 0x0a, 0x91, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf2, 0x09, 0x30, 0xf2, 0x0a, 0xf5, 0xf2, 0x09, 0xff, 0xf2, 0x8b, 0xff, 0xf4, 0x10, 0xff, 0xf2, 0x8b, 0xff, 0xf2, 0x09, 0xff, 0xf2, 0x0a, 0xf5, 0xf2, 0x09, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf2, 0x0a, 0xb2, 0xf2, 0x0a, 0xff, 0xf1, 0x68, 0xff, 0xfd, 0x75, 0xff, 0xff, 0xfe, 0xff, 0xfd, 0x75, 0xff, 0xf1, 0x68, 0xff, 0xf2, 0x0a, 0xff, 0xf2, 0x0a, 0xb2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf2, 0x0a, 0x54, 0xf2, 0x0a, 0xff, 0xf2, 0x0a, 0xff, 0xf1, 0x48, 0xff, 0xfd, 0xf7, 0xff, 0xff, 0xff, 0xff, 0xfd, 0xf7, 0xff, 0xf1, 0x48, 0xff, 0xf2, 0x0a, 0xff, 0xf2, 0x0a, 0xff, 0xf2, 0x0a, 0x54, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xea, 0x6a, 0x0a, 0xf2, 0x0a, 0xd2, 0xf2, 0x0a, 0xff, 0xf2, 0x0a, 0xff, 0xf1, 0x48, 0xff, 0xfd, 0xf7, 0xff, 0xff, 0xff, 0xff, 0xfd, 0xf7, 0xff, 0xf1, 0x48, 0xff, 0xf2, 0x0a, 0xff, 0xf2, 0x0a, 0xff, 0xf2, 0x0a, 0xd2, 0xea, 0x6a, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf2, 0x0a, 0x76, 0xf2, 0x0a, 0xff, 0xf2, 0x0a, 0xff, 0xf2, 0x0a, 0xff, 0xf1, 0x48, 0xff, 0xfd, 0xf7, 0xff, 0xff, 0xff, 0xff, 0xfd, 0xf7, 0xff, 0xf1, 0x48, 0xff, 0xf2, 0x0a, 0xff, 0xf2, 0x0a, 0xff, 0xf2, 0x0a, 0xff, 0xf2, 0x0a, 0x76, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf2, 0x29, 0x22, 0xf2, 0x0a, 0xeb, 0xf2, 0x0a, 0xff, 0xf2, 0x0a, 0xff, 0xf2, 0x0a, 0xff, 0xf1, 0x47, 0xff, 0xfd, 0xf7, 0xff, 0xff, 0xff, 0xff, 0xfd, 0xf7, 0xff, 0xf1, 0x47, 0xff, 0xf2, 0x0a, 0xff, 0xf2, 0x0a, 0xff, 0xf2, 0x0a, 0xff, 0xf2, 0x0a, 0xeb, 0xf2, 0x29, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf2, 0x09, 0x9c, 0xf2, 0x0a, 0xff, 0xf2, 0x0a, 0xff, 0xf2, 0x0a, 0xff, 0xf2, 0x0a, 0xff, 0xf1, 0x88, 0xff, 0xfd, 0x14, 0xff, 0xff, 0x9d, 0xff, 0xfd, 0x14, 0xff, 0xf1, 0x88, 0xff, 0xf2, 0x0a, 0xff, 0xf2, 0x0a, 0xff, 0xf2, 0x0a, 0xff, 0xf2, 0x0a, 0xff, 0xf2, 0x0a, 0x9b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf2, 0x0a, 0x3b, 0xf2, 0x0a, 0xfc, 0xf2, 0x0a, 0xff, 0xf2, 0x0a, 0xff, 0xf2, 0x0a, 0xff, 0xf2, 0x0a, 0xff, 0xf2, 0x0a, 0xff, 0xf1, 0xc9, 0xff, 0xf2, 0x4a, 0xff, 0xf1, 0xc9, 0xff, 0xf2, 0x0a, 0xff, 0xf2, 0x0a, 0xff, 0xf2, 0x0a, 0xff, 0xf2, 0x0a, 0xff, 0xf2, 0x0a, 0xff, 0xf2, 0x0a, 0xfc, 0xf2, 0x0a, 0x3b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0xfc, 0x10, 0x02, 0xf2, 0x0a, 0xbe, 0xf2, 0x0a, 0xff, 0xf2, 0x0a, 0xff, 0xf2, 0x0a, 0xff, 0xf2, 0x0a, 0xff, 0xf2, 0x0a, 0xff, 0xf1, 0xc9, 0xff, 0xf3, 0xaf, 0xff, 0xfd, 0xf7, 0xff, 0xf3, 0xaf, 0xff, 0xf1, 0xc9, 0xff, 0xf2, 0x0a, 0xff, 0xf2, 0x0a, 0xff, 0xf2, 0x0a, 0xff, 0xf2, 0x0a, 0xff, 0xf2, 0x0a, 0xff, 0xf2, 0x0a, 0xbe, 0xfc, 0x10, 0x02, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0xf2, 0x09, 0x5c, 0xf2, 0x0a, 0xff, 0xf2, 0x0a, 0xff, 0xf2, 0x0a, 0xff, 0xf2, 0x0a, 0xff, 0xf2, 0x0a, 0xff, 0xf2, 0x0a, 0xff, 0xf1, 0x48, 0xff, 0xfd, 0xb6, 0xff, 0xff, 0xff, 0xff, 0xfd, 0xb6, 0xff, 0xf1, 0x48, 0xff, 0xf2, 0x0a, 0xff, 0xf2, 0x0a, 0xff, 0xf2, 0x0a, 0xff, 0xf2, 0x0a, 0xff, 0xf2, 0x0a, 0xff, 0xf2, 0x0a, 0xff, 0xf2, 0x09, 0x5c, 0x00, 0x00, 0x00,
  0xe2, 0x08, 0x08, 0xf2, 0x0a, 0xd1, 0xf2, 0x0a, 0xff, 0xf2, 0x0a, 0xff, 0xf2, 0x0a, 0xff, 0xf2, 0x0a, 0xff, 0xf2, 0x0a, 0xff, 0xf2, 0x0a, 0xff, 0xf1, 0xe9, 0xff, 0xf3, 0x4d, 0xff, 0xfd, 0x75, 0xff, 0xf3, 0x4d, 0xff, 0xf1, 0xe9, 0xff, 0xf2, 0x0a, 0xff, 0xf2, 0x0a, 0xff, 0xf2, 0x0a, 0xff, 0xf2, 0x0a, 0xff, 0xf2, 0x0a, 0xff, 0xf2, 0x0a, 0xff, 0xf2, 0x0a, 0xd1, 0xe2, 0x08, 0x08,
  0xe1, 0xcb, 0x09, 0xf2, 0x0a, 0xd3, 0xf2, 0x0a, 0xff, 0xf2, 0x0a, 0xff, 0xf2, 0x0a, 0xff, 0xf2, 0x0a, 0xff, 0xf2, 0x0a, 0xff, 0xf2, 0x0a, 0xff, 0xf2, 0x0a, 0xff, 0xf1, 0xe9, 0xff, 0xf1, 0x88, 0xff, 0xf1, 0xe9, 0xff, 0xf2, 0x0a, 0xff, 0xf2, 0x0a, 0xff, 0xf2, 0x0a, 0xff, 0xf2, 0x0a, 0xff, 0xf2, 0x0a, 0xff, 0xf2, 0x0a, 0xff, 0xf2, 0x0a, 0xff, 0xf2, 0x0a, 0xd3, 0xe1, 0xcb, 0x09,
  0x00, 0x00, 0x00, 0xf2, 0x0a, 0x35, 0xf2, 0x0a, 0x9b, 0xf2, 0x0a, 0xa2, 0xf2, 0x0a, 0xa1, 0xf2, 0x0a, 0xa1, 0xf2, 0x0a, 0xa1, 0xf2, 0x0a, 0xa1, 0xf2, 0x0a, 0xa1, 0xf2, 0x0a, 0xa1, 0xf2, 0x0a, 0xa1, 0xf2, 0x0a, 0xa1, 0xf2, 0x0a, 0xa1, 0xf2, 0x0a, 0xa1, 0xf2, 0x0a, 0xa1, 0xf2, 0x0a, 0xa1, 0xf2, 0x0a, 0xa1, 0xf2, 0x0a, 0xa2, 0xf2, 0x0a, 0x9b, 0xf2, 0x0a, 0x35, 0x00, 0x00, 0x00,
#endif
#if LV_COLOR_DEPTH == 32
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46, 0x46, 0xe8, 0x0b, 0x4d, 0x3f, 0xec, 0x35, 0x46, 0x46, 0xe8, 0x0b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x40, 0xdf, 0x08, 0x4c, 0x40, 0xee, 0xb8, 0x4c, 0x40, 0xee, 0xff, 0x4c, 0x40, 0xee, 0xb8, 0x40, 0x40, 0xdf, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4c, 0x40, 0xef, 0x6f, 0x4c, 0x40, 0xee, 0xff, 0x4c, 0x40, 0xee, 0xff, 0x4c, 0x40, 0xee, 0xff, 0x4c, 0x40, 0xef, 0x6f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x47, 0x3d, 0xeb, 0x19, 0x4c, 0x3f, 0xee, 0xe5, 0x4c, 0x40, 0xee, 0xff, 0x4c, 0x40, 0xee, 0xff, 0x4c, 0x40, 0xee, 0xff, 0x4c, 0x3f, 0xee, 0xe5, 0x47, 0x3d, 0xeb, 0x19, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4c, 0x3f, 0xed, 0x91, 0x4c, 0x40, 0xee, 0xff, 0x4a, 0x3e, 0xee, 0xff, 0x43, 0x33, 0xee, 0xff, 0x4a, 0x3e, 0xee, 0xff, 0x4c, 0x40, 0xee, 0xff, 0x4c, 0x3f, 0xed, 0x91, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4a, 0x40, 0xef, 0x30, 0x4c, 0x3f, 0xee, 0xf5, 0x4b, 0x3e, 0xee, 0xff, 0x58, 0x4f, 0xee, 0xff, 0x81, 0x80, 0xf1, 0xff, 0x58, 0x4f, 0xee, 0xff, 0x4b, 0x3e, 0xee, 0xff, 0x4c, 0x3f, 0xee, 0xf5, 0x4a, 0x40, 0xef, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4c, 0x40, 0xee, 0xb2, 0x4c, 0x40, 0xee, 0xff, 0x3e, 0x2b, 0xed, 0xff, 0xa9, 0xad, 0xf5, 0xff, 0xf1, 0xfb, 0xff, 0xff, 0xa9, 0xad, 0xf5, 0xff, 0x3e, 0x2b, 0xed, 0xff, 0x4c, 0x40, 0xee, 0xff, 0x4c, 0x40, 0xee, 0xb2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4c, 0x40, 0xed, 0x54, 0x4c, 0x40, 0xee, 0xff, 0x4c, 0x40, 0xee, 0xff, 0x3c, 0x26, 0xed, 0xff, 0xb6, 0xbc, 0xf7, 0xff, 0xf5, 0xff, 0xff, 0xff, 0xb6, 0xbc, 0xf7, 0xff, 0x3c, 0x26, 0xed, 0xff, 0x4c, 0x40, 0xee, 0xff, 0x4c, 0x40, 0xee, 0xff, 0x4c, 0x40, 0xed, 0x54, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4d, 0x4d, 0xe6, 0x0a, 0x4c, 0x40, 0xee, 0xd2, 0x4c, 0x40, 0xee, 0xff, 0x4c, 0x40, 0xee, 0xff, 0x3c, 0x27, 0xed, 0xff, 0xb5, 0xba, 0xf7, 0xff, 0xf5, 0xff, 0xff, 0xff, 0xb5, 0xba, 0xf7, 0xff, 0x3c, 0x27, 0xed, 0xff, 0x4c, 0x40, 0xee, 0xff, 0x4c, 0x40, 0xee, 0xff, 0x4c, 0x40, 0xee, 0xd2, 0x4d, 0x4d, 0xe6, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4c, 0x41, 0xee, 0x76, 0x4c, 0x40, 0xee, 0xff, 0x4c, 0x40, 0xee, 0xff, 0x4c, 0x40, 0xee, 0xff, 0x3c, 0x27, 0xed, 0xff, 0xb5, 0xba, 0xf7, 0xff, 0xf4, 0xff, 0xff, 0xff, 0xb5, 0xba, 0xf7, 0xff, 0x3c, 0x27, 0xed, 0xff, 0x4c, 0x40, 0xee, 0xff, 0x4c, 0x40, 0xee, 0xff, 0x4c, 0x40, 0xee, 0xff, 0x4c, 0x41, 0xee, 0x76, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4b, 0x43, 0xf0, 0x22, 0x4c, 0x40, 0xee, 0xeb, 0x4c, 0x40, 0xee, 0xff, 0x4c, 0x40, 0xee, 0xff, 0x4c, 0x40, 0xee, 0xff, 0x3b, 0x26, 0xed, 0xff, 0xb6, 0xbc, 0xf7, 0xff, 0xf6, 0xff, 0xff, 0xff, 0xb6, 0xbc, 0xf7, 0xff, 0x3b, 0x26, 0xed, 0xff, 0x4c, 0x40, 0xee, 0xff, 0x4c, 0x40, 0xee, 0xff, 0x4c, 0x40, 0xee, 0xff, 0x4c, 0x40, 0xee, 0xeb, 0x4b, 0x43, 0xf0, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4b, 0x40, 0xef, 0x9c, 0x4c, 0x40, 0xee, 0xff, 0x4c, 0x40, 0xee, 0xff, 0x4c, 0x40, 0xee, 0xff, 0x4c, 0x40, 0xee, 0xff, 0x40, 0x2f, 0xed, 0xff, 0x9d, 0x9f, 0xf4, 0xff, 0xe6, 0xf0, 0xfe, 0xff, 0x9d, 0x9f, 0xf4, 0xff, 0x40, 0x2f, 0xed, 0xff, 0x4c, 0x40, 0xee, 0xff, 0x4c, 0x40, 0xee, 0xff, 0x4c, 0x40, 0xee, 0xff, 0x4c, 0x40, 0xee, 0xff, 0x4c, 0x40, 0xef, 0x9b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4e, 0x41, 0xee, 0x3b, 0x4c, 0x40, 0xee, 0xfc, 0x4c, 0x40, 0xee, 0xff, 0x4c, 0x40, 0xee, 0xff, 0x4c, 0x40, 0xee, 0xff, 0x4c, 0x40, 0xee, 0xff, 0x4c, 0x40, 0xee, 0xff, 0x46, 0x38, 0xee, 0xff, 0x53, 0x49, 0xee, 0xff, 0x46, 0x38, 0xee, 0xff, 0x4c, 0x40, 0xee, 0xff, 0x4c, 0x40, 0xee, 0xff, 0x4c, 0x40, 0xee, 0xff, 0x4c, 0x40, 0xee, 0xff, 0x4c, 0x40, 0xee, 0xff, 0x4c, 0x40, 0xee, 0xfc, 0x4e, 0x41, 0xee, 0x3b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0xff, 0x02, 0x4d, 0x40, 0xee, 0xbe, 0x4c, 0x40, 0xee, 0xff, 0x4c, 0x40, 0xee, 0xff, 0x4c, 0x40, 0xee, 0xff, 0x4c, 0x40, 0xee, 0xff, 0x4c, 0x40, 0xee, 0xff, 0x47, 0x39, 0xee, 0xff, 0x75, 0x73, 0xf0, 0xff, 0xb6, 0xbc, 0xf7, 0xff, 0x75, 0x73, 0xf0, 0xff, 0x47, 0x39, 0xee, 0xff, 0x4c, 0x40, 0xee, 0xff, 0x4c, 0x40, 0xee, 0xff, 0x4c, 0x40, 0xee, 0xff, 0x4c, 0x40, 0xee, 0xff, 0x4c, 0x40, 0xee, 0xff, 0x4d, 0x40, 0xee, 0xbe, 0x80, 0x80, 0xff, 0x02, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x4b, 0x40, 0xee, 0x5c, 0x4c, 0x40, 0xee, 0xff, 0x4c, 0x40, 0xee, 0xff, 0x4c, 0x40, 0xee, 0xff, 0x4c, 0x40, 0xee, 0xff, 0x4c, 0x40, 0xee, 0xff, 0x4c, 0x40, 0xee, 0xff, 0x3c, 0x28, 0xed, 0xff, 0xae, 0xb3, 0xf6, 0xff, 0xfa, 0xff, 0xff, 0xff, 0xae, 0xb3, 0xf6, 0xff, 0x3c, 0x28, 0xed, 0xff, 0x4c, 0x40, 0xee, 0xff, 0x4c, 0x40, 0xee, 0xff, 0x4c, 0x40, 0xee, 0xff, 0x4c, 0x40, 0xee, 0xff, 0x4c, 0x40, 0xee, 0xff, 0x4c, 0x40, 0xee, 0xff, 0x4b, 0x40, 0xee, 0x5c, 0x00, 0x00, 0x00, 0x00,
  0x40, 0x40, 0xdf, 0x08, 0x4c, 0x3f, 0xee, 0xd1, 0x4c, 0x40, 0xee, 0xff, 0x4c, 0x40, 0xee, 0xff, 0x4c, 0x40, 0xee, 0xff, 0x4c, 0x40, 0xee, 0xff, 0x4c, 0x40, 0xee, 0xff, 0x4c, 0x40, 0xee, 0xff, 0x49, 0x3c, 0xee, 0xff, 0x6a, 0x66, 0xf0, 0xff, 0xa6, 0xaa, 0xf5, 0xff, 0x6a, 0x66, 0xf0, 0xff, 0x49, 0x3c, 0xee, 0xff, 0x4c, 0x40, 0xee, 0xff, 0x4c, 0x40, 0xee, 0xff, 0x4c, 0x40, 0xee, 0xff, 0x4c, 0x40, 0xee, 0xff, 0x4c, 0x40, 0xee, 0xff, 0x4c, 0x40, 0xee, 0xff, 0x4c, 0x3f, 0xee, 0xd1, 0x40, 0x40, 0xdf, 0x08,
  0x55, 0x39, 0xe3, 0x09, 0x4c, 0x40, 0xee, 0xd3, 0x4c, 0x40, 0xee, 0xff, 0x4c, 0x40, 0xee, 0xff, 0x4c, 0x40, 0xee, 0xff, 0x4c, 0x40, 0xee, 0xff, 0x4c, 0x40, 0xee, 0xff, 0x4c, 0x40, 0xee, 0xff, 0x4c, 0x40, 0xee, 0xff, 0x49, 0x3c, 0xee, 0xff, 0x40, 0x2e, 0xed, 0xff, 0x49, 0x3c, 0xee, 0xff, 0x4c, 0x40, 0xee, 0xff, 0x4c, 0x40, 0xee, 0xff, 0x4c, 0x40, 0xee, 0xff, 0x4c, 0x40, 0xee, 0xff, 0x4c, 0x40, 0xee, 0xff, 0x4c, 0x40, 0xee, 0xff, 0x4c, 0x40, 0xee, 0xff, 0x4c, 0x40, 0xee, 0xd3, 0x55, 0x39, 0xe3, 0x09,
  0x00, 0x00, 0x00, 0x00, 0x4d, 0x3f, 0xec, 0x35, 0x4c, 0x40, 0xef, 0x9b, 0x4c, 0x41, 0xee, 0xa2, 0x4c, 0x3f, 0xee, 0xa1, 0x4c, 0x3f, 0xee, 0xa1, 0x4c, 0x3f, 0xee, 0xa1, 0x4c, 0x3f, 0xee, 0xa1, 0x4c, 0x3f, 0xee, 0xa1, 0x4c, 0x3f, 0xee, 0xa1, 0x4c, 0x3f, 0xee, 0xa1, 0x4c, 0x3f, 0xee, 0xa1, 0x4c, 0x3f, 0xee, 0xa1, 0x4c, 0x3f, 0xee, 0xa1, 0x4c, 0x3f, 0xee, 0xa1, 0x4c, 0x3f, 0xee, 0xa1, 0x4c, 0x3f, 0xee, 0xa1, 0x4c, 0x41, 0xee, 0xa2, 0x4c, 0x40, 0xef, 0x9b, 0x4d, 0x3f, 0xec, 0x35, 0x00, 0x00, 0x00, 0x00,
#endif
};

const lv_img_dsc_t icon_warning = {
  .header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA,
  .header.always_zero = 0,
  .header.reserved = 0,
  .header.w = 21,
  .header.h = 18,
  .data_size = 378 * LV_IMG_PX_SIZE_ALPHA_BYTE,
  .data = icon_warning_map,
};
