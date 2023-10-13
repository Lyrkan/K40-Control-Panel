<p align="center">
  <img width="640" height="178" src="images/banner.png">
</p>

<h1 align="center">K40 Control Panel</h1>

<p align="center"><i>Control Panel for K40 laser cutters/engravers</i></p>

<p align="center">
    <img width="240" height="160" src="images/screenshot-status.jpg">
    <img width="240" height="160" src="images/screenshot-controls.jpg">
    <img width="240" height="160" src="images/screenshot-bed.jpg">
</p>

<p align="center">
    <a title="License" href="https://github.com/Lyrkan/K40-Control-Panel/blob/master/LICENSE"><img src="https://img.shields.io/github/license/Lyrkan/K40-Control-Panel.svg"></a>
    <img alt="Build status" src="https://github.com/Lyrkan/K40-Control-Panel/actions/workflows/build-main.yml/badge.svg">
    <a title="Last release"><img src="https://img.shields.io/github/release/Lyrkan/K40-Control-Panel.svg"></a>
</p>

## Overview

What it **CAN** do:

-   Monitor input/output coolant flow and temperature (using "YF-B4"-type sensors and thermistors)
-   Monitor lids opening (using two microswitches)
-   Monitor for fire (using an LM393 IR flame sensor module)
-   Control a 3 channels relays board for:
    -   CO2 laser activation (the main switch/interlock on an vanilla K40, not the "fire" button)
    -   Air-assist activation
    -   Alarm activation
-   Control 12V lights
-   Control 5V laser diodes
-   Control a motorized bed (based on [this design](https://civade.com/post/2020/08/23/D%c3%a9coupe-laser-CO2-K40-:-R%c3%a9alisation-d-un-lit-motoris%c3%a9), but it should work with other ones as long as the stepper can be controled using a DRV8825 driver)
-   Expose sensors/state data through an API **(unstable)**

What it **CANNOT** do yet:

-   Move the laser head
-   Trigger the CO2 laser

Also, don't expect an ultra smooth experience, the ESP32 being a bit underpowered for a screen of that size the user interface can be a bit laggy sometimes (eg. during screen transitions or scrolling). It should still be fully usable though.

## BOM

For the control panel:

-   1x NodeMCU-32 (38 pins)
-   1x ILI9488 3.95" LCD screen
-   1x DRV8825 stepper driver
-   1x L7805CV 5V regulator
-   2x IRLML6244 N-channel MOSFETs
-   2x 1N4007 diodes
-   5x 10kΩ 1/4W R1206 SMD resistors
-   2x 220Ω 1/4W R1206 SMD resistors
-   1x 0.1uF electrolytic capacitor
-   1x 100uF electrolytic capacitor
-   1x 2P PCB terminal blocks (5mm pitch)
-   some 2.54mm pin headers / XH connectors / jumpers

Other things you may need:

-   1x motorized bed with a NEMA17 stepper and a limit switch
-   1x 3 channel relay board
-   1x air compressor for air assist
-   2x YF-B4 flow sensor with a thermistor (preferably one with a 50kΩ resistance at 25°C and B=3950K)
-   1x LM393 IR flame sensor module
-   2x micro switches
-   1x (or 2x depending on your needs) 5V laser diodes
-   1x 12V LED light (for the enclosure)
-   1x 12V blinking light (for the alarm)

## Building the firmware

Make sure you have Python 3.x installed on your system, then run:

```sh
# Install/update Platform.io
pip install --upgrade platformio

# Build the firmware
pio build
```

Then flash your ESP32 using the `upload` target.

OTA updates are available using the `OTA` target. In order to use it copy the `.env.dist` file to `.env` and change the values of `OTA_UPLOAD_URL`, `OTA_LOGIN` and `OTA_PASSWORD` in it accordingly to the settings of your control panel.

You can also take a `.bin` file and upload it through the ElegantOTA interface available at `http://<YOUR_PANEL_IP>/update`.

## PCB

![PCB](/images/pcb-3d.png)

Production files can be found in the [gerber](/gerber) folder of this repository.

The design fits on a 100x100mm PCB, so you should be able to get it built for really cheap by online suppliers.

## Wiring

Coming soon™

## Status APIs

### System/firmware information

```
GET http://<YOUR_PANEL_IP>/api/info
```

```json
{
    "firmware": {
        "version": "9da28a82",
        "build_date": "Apr 30 2023 18:31:43"
    },
    "system": {
        "chip": {
            "model": "ESP32-D0WDQ5",
            "revision": 1
        },
        "heap": {
            "free": 80268,
            "total": 257148
        },
        "cpu": {
            "freq_mhz": 240,
            "load_percent": {
                "core_0": 0,
                "core_1": 0.100000001
            }
        }
    }
}
```

### Sensors status

```
GET http://<YOUR_PANEL_IP>/api/sensors
```

```json
{
    "sensors": {
        "cooling": {
            "flow": {
                "input": 5.619999886,
                "output": 5.60124556
            },
            "temp": {
                "input": 18.89999962,
                "output": 21.3
            }
        },
        "lids": {
            "front": "opened",
            "back": "closed"
        },
        "flame_sensor": {
            "triggered": false
        }
    }
}
```

### Alerts state

```
GET http://<YOUR_PANEL_IP>/api/alerts
```

```json
{
    "alerts": {
        "cooling": false,
        "lids": true,
        "flame_sensor": false
    }
}
```

### Relays/MOSFETs activation state

```
GET http://<YOUR_PANEL_IP>/api/relays
```

```json
{
    "relays": {
        "interlock": false,
        "air_assist": true,
        "cooling": true,
        "alarm": false,
        "lights": true,
        "beam_preview": true
    }
}
```
