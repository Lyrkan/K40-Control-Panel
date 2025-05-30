#ifndef LGFX_H
#define LGFX_H

#define LGFX_USE_V1
#include <LovyanGFX.hpp>

#include "UI/display.h"

class LGFX : public lgfx::LGFX_Device {

#if DISPLAY_DRIVER == DISPLAY_DRIVER_ILI9488
    lgfx::Panel_ILI9488 _panel_instance;
#elif DISPLAY_DRIVER == DISPLAY_DRIVER_ST7796
    lgfx::Panel_ST7796 _panel_instance;
#else
#error "Unsupported display driver"
#endif

    lgfx::Light_PWM _light_instance;
    lgfx::Bus_SPI _bus_instance;
    lgfx::Touch_XPT2046 _touch_instance;

  public:
    LGFX(void) {
        {
            auto cfg = _bus_instance.config();

            cfg.spi_host = VSPI_HOST;
            cfg.spi_mode = 0;

#if DISPLAY_DRIVER == DISPLAY_DRIVER_ST7796
            cfg.freq_write = 80000000;
#else
            cfg.freq_write = 40000000;
#endif

            cfg.freq_read = 16000000;
            cfg.spi_3wire = false;
            cfg.use_lock = true;
            cfg.dma_channel = SPI_DMA_CH_AUTO;
            cfg.pin_sclk = 18;
            cfg.pin_mosi = 23;
            cfg.pin_miso = 19;
            cfg.pin_dc = 2;

            _bus_instance.config(cfg);
            _panel_instance.setBus(&_bus_instance);
        }

        {
            auto cfg = _panel_instance.config();
            cfg.pin_cs = 15;
            cfg.pin_rst = -1;
            cfg.pin_busy = -1;
            cfg.panel_width = DISPLAY_SCREEN_HEIGHT;
            cfg.panel_height = DISPLAY_SCREEN_WIDTH;
            cfg.offset_x = 0;
            cfg.offset_y = 0;
            cfg.dummy_read_bits = 1;
            cfg.readable = true;
            cfg.invert = false;
            cfg.rgb_order = false;
            cfg.dlen_16bit = false;
            cfg.bus_shared = false;

#if DISPLAY_DRIVER == DISPLAY_DRIVER_ST7796
            cfg.offset_rotation = 3;
#else
            cfg.offset_rotation = 1;
#endif

            _panel_instance.config(cfg);
        }

        {
            auto cfg = _light_instance.config();
            cfg.pin_bl = 14;
            _light_instance.config(cfg);
            _panel_instance.setLight(&_light_instance);
        }

        {
            auto cfg = _touch_instance.config();

            cfg.x_min = 0;
            cfg.x_max = DISPLAY_SCREEN_HEIGHT - 1;
            cfg.y_min = 0;
            cfg.y_max = DISPLAY_SCREEN_WIDTH - 1;
            cfg.pin_int = -1;
            cfg.bus_shared = false;
            cfg.offset_rotation = 0;

            cfg.spi_host = VSPI_HOST;
            cfg.freq = 1000000;
            cfg.pin_sclk = 18;
            cfg.pin_mosi = 23;
            cfg.pin_miso = 19;
            cfg.pin_cs = 21;

            _touch_instance.config(cfg);
            _panel_instance.setTouch(&_touch_instance);
        }

        setPanel(&_panel_instance);
    }
};

#endif
