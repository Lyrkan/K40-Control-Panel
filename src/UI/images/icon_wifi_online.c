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

#ifndef LV_ATTRIBUTE_IMG_ICON_WIFI_ONLINE
#define LV_ATTRIBUTE_IMG_ICON_WIFI_ONLINE
#endif

const LV_ATTRIBUTE_MEM_ALIGN LV_ATTRIBUTE_LARGE_CONST LV_ATTRIBUTE_IMG_ICON_WIFI_ONLINE uint8_t icon_wifi_online_map[] = {
#if LV_COLOR_DEPTH == 1 || LV_COLOR_DEPTH == 8
  /*Pixel format: Alpha 8 bit, Red: 3 bit, Green: 3 bit, Blue: 2 bit*/
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x25, 0x00, 0x3c, 0x00, 0x3c, 0x00, 0x25, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x48, 0x00, 0xad, 0x00, 0xf2, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xf2, 0x00, 0xad, 0x00, 0x48, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x27, 0x00, 0xc3, 0x00, 0xff, 0x00, 0xff, 0x00, 0xed, 0x00, 0xb7, 0x00, 0x9b, 0x00, 0x9b, 0x00, 0xb7, 0x00, 0xed, 0x00, 0xff, 0x00, 0xff, 0x00, 0xc3, 0x00, 0x27, 0x00, 0x00,
  0x00, 0x49, 0x00, 0xf2, 0x00, 0xff, 0x00, 0xc7, 0x00, 0x4b, 0x00, 0x04, 0x00, 0x11, 0x00, 0x30, 0x00, 0x30, 0x00, 0x11, 0x00, 0x04, 0x00, 0x4b, 0x00, 0xc7, 0x00, 0xff, 0x00, 0xf2, 0x00, 0x49,
  0x00, 0xf1, 0x00, 0xfc, 0x00, 0x73, 0x00, 0x05, 0x00, 0x63, 0x00, 0xcd, 0x00, 0xfe, 0x00, 0xff, 0x00, 0xff, 0x00, 0xfe, 0x00, 0xcd, 0x00, 0x63, 0x00, 0x05, 0x00, 0x73, 0x00, 0xfc, 0x00, 0xf1,
  0x00, 0x59, 0x00, 0x46, 0x00, 0x1b, 0x00, 0xc7, 0x00, 0xff, 0x00, 0xff, 0x00, 0xd3, 0x00, 0xa9, 0x00, 0xa9, 0x00, 0xd3, 0x00, 0xff, 0x00, 0xff, 0x00, 0xc7, 0x00, 0x1b, 0x00, 0x46, 0x00, 0x59,
  0x00, 0x00, 0x00, 0x00, 0x00, 0xc1, 0x00, 0xff, 0x00, 0xbf, 0x00, 0x2f, 0x00, 0x03, 0x00, 0x24, 0x00, 0x24, 0x00, 0x03, 0x00, 0x2f, 0x00, 0xbf, 0x00, 0xff, 0x00, 0xc1, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x63, 0x00, 0x81, 0x00, 0x06, 0x00, 0x77, 0x00, 0xea, 0x00, 0xff, 0x00, 0xff, 0x00, 0xea, 0x00, 0x78, 0x00, 0x06, 0x00, 0x81, 0x00, 0x63, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x8c, 0x00, 0xff, 0x00, 0xf7, 0x00, 0xb8, 0x00, 0xb8, 0x00, 0xf7, 0x00, 0xff, 0x00, 0x8c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x64, 0x00, 0xb7, 0x00, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x00, 0xb6, 0x00, 0x64, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x21, 0x00, 0xc6, 0x00, 0xc6, 0x00, 0x21, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x88, 0x00, 0xff, 0x00, 0xff, 0x00, 0x88, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4d, 0x00, 0xfd, 0x00, 0xfd, 0x00, 0x4d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x24, 0x00, 0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
#endif
#if LV_COLOR_DEPTH == 16 && LV_COLOR_16_SWAP == 0
  /*Pixel format: Alpha 8 bit, Red: 5 bit, Green: 6 bit, Blue: 5 bit*/
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x25, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x25, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x48, 0x00, 0x00, 0xad, 0x00, 0x00, 0xf2, 0x00, 0x00, 0xff, 0x00, 0x00, 0xff, 0x00, 0x00, 0xff, 0x00, 0x00, 0xff, 0x00, 0x00, 0xf2, 0x00, 0x00, 0xad, 0x00, 0x00, 0x48, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x27, 0x00, 0x00, 0xc3, 0x00, 0x00, 0xff, 0x00, 0x00, 0xff, 0x00, 0x00, 0xed, 0x00, 0x00, 0xb7, 0x00, 0x00, 0x9b, 0x00, 0x00, 0x9b, 0x00, 0x00, 0xb7, 0x00, 0x00, 0xed, 0x00, 0x00, 0xff, 0x00, 0x00, 0xff, 0x00, 0x00, 0xc3, 0x00, 0x00, 0x27, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x49, 0x00, 0x00, 0xf2, 0x00, 0x00, 0xff, 0x00, 0x00, 0xc7, 0x00, 0x00, 0x4b, 0x00, 0x00, 0x04, 0x00, 0x00, 0x11, 0x00, 0x00, 0x30, 0x00, 0x00, 0x30, 0x00, 0x00, 0x11, 0x00, 0x00, 0x04, 0x00, 0x00, 0x4b, 0x00, 0x00, 0xc7, 0x00, 0x00, 0xff, 0x00, 0x00, 0xf2, 0x00, 0x00, 0x49,
  0x00, 0x00, 0xf1, 0x00, 0x00, 0xfc, 0x00, 0x00, 0x73, 0x00, 0x00, 0x05, 0x00, 0x00, 0x63, 0x00, 0x00, 0xcd, 0x00, 0x00, 0xfe, 0x00, 0x00, 0xff, 0x00, 0x00, 0xff, 0x00, 0x00, 0xfe, 0x00, 0x00, 0xcd, 0x00, 0x00, 0x63, 0x00, 0x00, 0x05, 0x00, 0x00, 0x73, 0x00, 0x00, 0xfc, 0x00, 0x00, 0xf1,
  0x00, 0x00, 0x59, 0x00, 0x00, 0x46, 0x00, 0x00, 0x1b, 0x00, 0x00, 0xc7, 0x00, 0x00, 0xff, 0x00, 0x00, 0xff, 0x00, 0x00, 0xd3, 0x00, 0x00, 0xa9, 0x00, 0x00, 0xa9, 0x00, 0x00, 0xd3, 0x00, 0x00, 0xff, 0x00, 0x00, 0xff, 0x00, 0x00, 0xc7, 0x00, 0x00, 0x1b, 0x00, 0x00, 0x46, 0x00, 0x00, 0x59,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc1, 0x00, 0x00, 0xff, 0x00, 0x00, 0xbf, 0x00, 0x00, 0x2f, 0x00, 0x00, 0x03, 0x00, 0x00, 0x24, 0x00, 0x00, 0x24, 0x00, 0x00, 0x03, 0x00, 0x00, 0x2f, 0x00, 0x00, 0xbf, 0x00, 0x00, 0xff, 0x00, 0x00, 0xc1, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x63, 0x00, 0x00, 0x81, 0x00, 0x00, 0x06, 0x00, 0x00, 0x77, 0x00, 0x00, 0xea, 0x00, 0x00, 0xff, 0x00, 0x00, 0xff, 0x00, 0x00, 0xea, 0x00, 0x00, 0x78, 0x00, 0x00, 0x06, 0x00, 0x00, 0x81, 0x00, 0x00, 0x63, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x8c, 0x00, 0x00, 0xff, 0x00, 0x00, 0xf7, 0x00, 0x00, 0xb8, 0x00, 0x00, 0xb8, 0x00, 0x00, 0xf7, 0x00, 0x00, 0xff, 0x00, 0x00, 0x8c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x64, 0x00, 0x00, 0xb7, 0x00, 0x00, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x00, 0x00, 0xb6, 0x00, 0x00, 0x64, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x21, 0x00, 0x00, 0xc6, 0x00, 0x00, 0xc6, 0x00, 0x00, 0x21, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x88, 0x00, 0x00, 0xff, 0x00, 0x00, 0xff, 0x00, 0x00, 0x88, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4d, 0x00, 0x00, 0xfd, 0x00, 0x00, 0xfd, 0x00, 0x00, 0x4d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x24, 0x00, 0x00, 0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
#endif
#if LV_COLOR_DEPTH == 16 && LV_COLOR_16_SWAP != 0
  /*Pixel format: Alpha 8 bit, Red: 5 bit, Green: 6 bit, Blue: 5 bit  BUT the 2  color bytes are swapped*/
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x25, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x25, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x48, 0x00, 0x00, 0xad, 0x00, 0x00, 0xf2, 0x00, 0x00, 0xff, 0x00, 0x00, 0xff, 0x00, 0x00, 0xff, 0x00, 0x00, 0xff, 0x00, 0x00, 0xf2, 0x00, 0x00, 0xad, 0x00, 0x00, 0x48, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x27, 0x00, 0x00, 0xc3, 0x00, 0x00, 0xff, 0x00, 0x00, 0xff, 0x00, 0x00, 0xed, 0x00, 0x00, 0xb7, 0x00, 0x00, 0x9b, 0x00, 0x00, 0x9b, 0x00, 0x00, 0xb7, 0x00, 0x00, 0xed, 0x00, 0x00, 0xff, 0x00, 0x00, 0xff, 0x00, 0x00, 0xc3, 0x00, 0x00, 0x27, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x49, 0x00, 0x00, 0xf2, 0x00, 0x00, 0xff, 0x00, 0x00, 0xc7, 0x00, 0x00, 0x4b, 0x00, 0x00, 0x04, 0x00, 0x00, 0x11, 0x00, 0x00, 0x30, 0x00, 0x00, 0x30, 0x00, 0x00, 0x11, 0x00, 0x00, 0x04, 0x00, 0x00, 0x4b, 0x00, 0x00, 0xc7, 0x00, 0x00, 0xff, 0x00, 0x00, 0xf2, 0x00, 0x00, 0x49,
  0x00, 0x00, 0xf1, 0x00, 0x00, 0xfc, 0x00, 0x00, 0x73, 0x00, 0x00, 0x05, 0x00, 0x00, 0x63, 0x00, 0x00, 0xcd, 0x00, 0x00, 0xfe, 0x00, 0x00, 0xff, 0x00, 0x00, 0xff, 0x00, 0x00, 0xfe, 0x00, 0x00, 0xcd, 0x00, 0x00, 0x63, 0x00, 0x00, 0x05, 0x00, 0x00, 0x73, 0x00, 0x00, 0xfc, 0x00, 0x00, 0xf1,
  0x00, 0x00, 0x59, 0x00, 0x00, 0x46, 0x00, 0x00, 0x1b, 0x00, 0x00, 0xc7, 0x00, 0x00, 0xff, 0x00, 0x00, 0xff, 0x00, 0x00, 0xd3, 0x00, 0x00, 0xa9, 0x00, 0x00, 0xa9, 0x00, 0x00, 0xd3, 0x00, 0x00, 0xff, 0x00, 0x00, 0xff, 0x00, 0x00, 0xc7, 0x00, 0x00, 0x1b, 0x00, 0x00, 0x46, 0x00, 0x00, 0x59,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc1, 0x00, 0x00, 0xff, 0x00, 0x00, 0xbf, 0x00, 0x00, 0x2f, 0x00, 0x00, 0x03, 0x00, 0x00, 0x24, 0x00, 0x00, 0x24, 0x00, 0x00, 0x03, 0x00, 0x00, 0x2f, 0x00, 0x00, 0xbf, 0x00, 0x00, 0xff, 0x00, 0x00, 0xc1, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x63, 0x00, 0x00, 0x81, 0x00, 0x00, 0x06, 0x00, 0x00, 0x77, 0x00, 0x00, 0xea, 0x00, 0x00, 0xff, 0x00, 0x00, 0xff, 0x00, 0x00, 0xea, 0x00, 0x00, 0x78, 0x00, 0x00, 0x06, 0x00, 0x00, 0x81, 0x00, 0x00, 0x63, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x8c, 0x00, 0x00, 0xff, 0x00, 0x00, 0xf7, 0x00, 0x00, 0xb8, 0x00, 0x00, 0xb8, 0x00, 0x00, 0xf7, 0x00, 0x00, 0xff, 0x00, 0x00, 0x8c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x64, 0x00, 0x00, 0xb7, 0x00, 0x00, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x00, 0x00, 0xb6, 0x00, 0x00, 0x64, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x21, 0x00, 0x00, 0xc6, 0x00, 0x00, 0xc6, 0x00, 0x00, 0x21, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x88, 0x00, 0x00, 0xff, 0x00, 0x00, 0xff, 0x00, 0x00, 0x88, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4d, 0x00, 0x00, 0xfd, 0x00, 0x00, 0xfd, 0x00, 0x00, 0x4d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x24, 0x00, 0x00, 0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
#endif
#if LV_COLOR_DEPTH == 32
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x25, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x00, 0x25, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x48, 0x00, 0x00, 0x00, 0xad, 0x00, 0x00, 0x00, 0xf2, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xf2, 0x00, 0x00, 0x00, 0xad, 0x00, 0x00, 0x00, 0x48, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x27, 0x00, 0x00, 0x00, 0xc3, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xed, 0x00, 0x00, 0x00, 0xb7, 0x00, 0x00, 0x00, 0x9b, 0x00, 0x00, 0x00, 0x9b, 0x00, 0x00, 0x00, 0xb7, 0x00, 0x00, 0x00, 0xed, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xc3, 0x00, 0x00, 0x00, 0x27, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x49, 0x00, 0x00, 0x00, 0xf2, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xc7, 0x00, 0x00, 0x00, 0x4b, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x11, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x11, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x4b, 0x00, 0x00, 0x00, 0xc7, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xf2, 0x00, 0x00, 0x00, 0x49,
  0x00, 0x00, 0x00, 0xf1, 0x00, 0x00, 0x00, 0xfc, 0x00, 0x00, 0x00, 0x73, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x63, 0x00, 0x00, 0x00, 0xcd, 0x00, 0x00, 0x00, 0xfe, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xfe, 0x00, 0x00, 0x00, 0xcd, 0x00, 0x00, 0x00, 0x63, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x73, 0x00, 0x00, 0x00, 0xfc, 0x00, 0x00, 0x00, 0xf1,
  0x00, 0x00, 0x00, 0x59, 0x00, 0x00, 0x00, 0x46, 0x00, 0x00, 0x00, 0x1b, 0x00, 0x00, 0x00, 0xc7, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xd3, 0x00, 0x00, 0x00, 0xa9, 0x00, 0x00, 0x00, 0xa9, 0x00, 0x00, 0x00, 0xd3, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xc7, 0x00, 0x00, 0x00, 0x1b, 0x00, 0x00, 0x00, 0x46, 0x00, 0x00, 0x00, 0x59,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc1, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xbf, 0x00, 0x00, 0x00, 0x2f, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x24, 0x00, 0x00, 0x00, 0x24, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x2f, 0x00, 0x00, 0x00, 0xbf, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xc1, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x63, 0x00, 0x00, 0x00, 0x81, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x77, 0x00, 0x00, 0x00, 0xea, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xea, 0x00, 0x00, 0x00, 0x78, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x81, 0x00, 0x00, 0x00, 0x63, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x8c, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xf7, 0x00, 0x00, 0x00, 0xb8, 0x00, 0x00, 0x00, 0xb8, 0x00, 0x00, 0x00, 0xf7, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x8c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x64, 0x00, 0x00, 0x00, 0xb7, 0x00, 0x00, 0x00, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x00, 0x00, 0x00, 0xb6, 0x00, 0x00, 0x00, 0x64, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x21, 0x00, 0x00, 0x00, 0xc6, 0x00, 0x00, 0x00, 0xc6, 0x00, 0x00, 0x00, 0x21, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x88, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x88, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4d, 0x00, 0x00, 0x00, 0xfd, 0x00, 0x00, 0x00, 0xfd, 0x00, 0x00, 0x00, 0x4d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x24, 0x00, 0x00, 0x00, 0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
#endif
};

const lv_img_dsc_t icon_wifi_online = {
  .header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA,
  .header.always_zero = 0,
  .header.reserved = 0,
  .header.w = 16,
  .header.h = 16,
  .data_size = 256 * LV_IMG_PX_SIZE_ALPHA_BYTE,
  .data = icon_wifi_online_map,
};
