<p align="center">
  <img width="640" height="178" src="images/banner.png">
</p>

<h1 align="center">K40 Control Panel</h1>

<p align="center"><i>Control Panel for K40 laser cutters/engravers</i></p>

[![GitHub license](https://img.shields.io/github/license/Naereen/StrapDown.js.svg)](https://github.com/Lyrkan/K40-Control-Panel/blob/master/LICENSE) ![Build status](https://github.com/Lyrkan/K40-Control-Panel/actions/workflows/build-main.yml/badge.svg) [![GitHub release](https://img.shields.io/github/release/Lyrkan/K40-Control-Panel.svg)](https://github.com/Lyrkan/K40-Control-Panel/releases/)

## Overview

What it **CAN** do:
* Monitor coolant flow and temperature (using a "YF-B4"-type sensor and a thermistor)
* Monitor lids opening (using two microswitches)
* Monitor 3 separate voltages (< 50V, they have to share the same ground as the control board)
* Monitor for fire (using an LM393 IR flame sensor module)
* Control a 6 channels relays board for:
    * CO2 laser activation (the main switch on an vanilla K40, not the "fire" button)
    * Air-assist activation
    * Cooling activation
    * Lights activation
    * Positioning laser diodes activation
    * Alarm activation
* Control a motorized bed (based on [this design](https://civade.com/post/2020/08/23/D%c3%a9coupe-laser-CO2-K40-:-R%c3%a9alisation-d-un-lit-motoris%c3%a9), but it should work with other ones as long as the stepper can be controled using a DRV8825 driver)
* Expose sensors/state data through an API

What it **CANNOT** do:
* Move the laser head
* Trigger the CO2 laser

Also, don't expect an ultra smooth experience, the ESP32 being a bit underpowered for a screen of that size the user interface can be a bit laggy sometimes (eg. during screen transitions or scrolling). It should still be fully usable though.

## BOM

For the control panel:
* 1x NodeMCU-32 (38 pins)
* 1x ILI9488 3.95" LCD screen
* 1x DRV8825 stepper driver
* 2x 10kΩ 1/4W resistor
* 3x 27kΩ 1/4W resistors
* 3x 470kΩ 1/4W resistors
* 1x 0.1uF electrolytic capacitor
* 1x 100uF electrolytic capacitor
* 3x 0.1uF ceramic capacitors
* 2x 2P PCB terminal blocks (5mm pitch)
* some 2.54mm pin headers / XH connectors

Other things you may need:
* 1x motorized bed with a NEMA17 stepper and a limit switch
* 1x 6 channel relay board
* 1x air compressor for air assist
* 1x YF-B4 flow sensor with a thermistor (preferably one with a 50kΩ resistance at 25°C and B=3950K)
* 1x LM393 IR flame sensor module
* 2x micro switches
* 1x (or 2x depending on your needs) 5V laser diodes
* 1x 12V blinking light

## Building the firmware

Make sure you have Python 3.x installed on your system, then run:

```sh
# Install/update Platform.io
pip install --upgrade platformio

# Build the firmware
pio build
```

Then flash your ESP32 using the `upload` target.

OTA updates are available using the `OTA` target. In order to use it copy the `.env.dist` file to `.env` and change the value of `OTA_UPLOAD_URL` in it accordingly to the IP of your control panel.

You can also take a `.bin` file and upload it through the ElegantOTA interface available at `http://<YOUR_PANEL_IP>/update`.

## Wiring

Coming soon™
