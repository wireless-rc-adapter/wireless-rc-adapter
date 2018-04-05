# Wireless RC Adapter
**Arduino game controller adapter for RC receivers up to 6 channels.**

There is no need to hook the transmitter with cables to the simulator device anymore. Instead plug your favorite RC receiver in the adapter and play wireless on TV, Desktop or even handheld device. It acts as an USB joystick, dispatching the channels values read from a regular RC receiver. Aimed to be fast and responsible, though simple to use with automatic calibration function. It can be useful to play simulator wireless from the computer with your favourite transmitter. 
Compatible with **Windows**, **Mac**, **Linux**, **Android**,**iOS**.

## Parts-list
Following parts are necessary to build the adapter:
 - [**Sparkfun Pro Micro** (5v/16Mhz)](http://www.ebay.com/sch/i.html?_from=R40&_trksid=p2050601.m570.l1313.TR0.TRC0.H0.Xatmega32u4.TRS0&_nkw=atmega32u4&_sacat=0)
 - [**6 CH RC receiver** (PWM, 5v)](http://www.ebay.com/sch/i.html?_from=R40&_trksid=p2050601.m570.l1313.TR3.TRC2.A0.H0.Xfs-r6b.TRS0&_nkw=fs-r6b&_sacat=0)
 - [**Tactile SPST switch** (momentary pushbutton)](http://www.ebay.com/sch/i.html?_from=R40&_trksid=p2050601.m570.l1313.TR10.TRC2.A0.H0.Xspst+switch+smd.TRS0&_nkw=spst+switch+smd&_sacat=0)
 - [**some jumper wires** (female-female)](http://www.ebay.com/sch/i.html?_from=R40&_trksid=p2050601.m570.l1313.TR12.TRC2.A0.H0.Xjumper+wire+arduino.TRS0&_nkw=jumper+wire+arduino&_sacat=0)

## Requirements
The code is **mostly written for Sparkfun Pro Micro**, since it is based on **ATmega 32U4** and can act as a game controller on USB. Although it may compile fine on other '32U4' boards also (eg. Leonardo). Compiling this code requires Arduino IDE v1.8.2 at least and 2 additional libraries.

[**Arduino Joystick Library**](http://github.com/MHeironimus/ArduinoJoystickLibrary)

[**PinChangeInterrupt Library**](http://github.com/NicoHood/PinChangeInterrupt)

## Wiring
![Wireless RC Adapter schematics](https://raw.githubusercontent.com/gregnau/wireless_rc_adapter/master/wiring.png)

## Manual
The Pro Micro boards usually have 2 leds side-by-side, which are sometimes labeles as the TX and RX. These are used to reflect the actual state of the adapter. On every startup flashing twice, then one of them is going of, the other one stays on. This means booted up successfully and ready to play. When both of them stays on, that means the adapter is in calibration mode.

### Calibration
The device needs to be calibrated on first use and recommended to continue to do it regularly. It happens automatically when there is no data in the eeprom or it is not readable. Otherwise it must be powered on while pushing the button to get into ```Calibration-mode```. The process is the same as everywhere: ```sticks/pots/switches on all channels must be moved to their extents```. Preferably more than once and with ```steady, slow motion```. During calibration, both leds on a Pro Micro should be lit up. When there is enough calibration data it will store it in the eeprom and switch to Live-mode automatically. ```It is also possible to finish calibration and save values by pressing the button once```.


### Debug
There is a ```DEBUG_ENABLE``` option available in the source file. When it is compiled with this flag set to ```TRUE```, then debug information will be printed to terminal. While Debug-mode is enabled there is no virtual joystick on os, only terminal. One can observe here the raw input of the channels.

---
> **Under Development 🚧 (pull-requests & issues are welcome!)**