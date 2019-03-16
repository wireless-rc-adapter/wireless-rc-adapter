#if !defined(SERIAL_DEBUG)
  #include "src/ArduinoJoystickLibrary/Joystick.h"

  #define HID_REPORT_ID 0x03  // Indicates the joystick's HID report ID. This value must be unique
                              // if you are creating multiple instances of Joystick. Do not use 
                              // 0x01 or 0x02 as they are used by the built-in Keyboard and Mouse libraries.
  #define JOYSTICK_TYPE 0x04  // Indicates the HID input device type. Supported values:
                                                 // JOYSTICK_TYPE_JOYSTICK or 0x04 - Joystick
                                                 // JOYSTICK_TYPE_GAMEPAD or 0x05 - Gamepad
                                                 // JOYSTICK_TYPE_MULTI_AXIS or 0x08 - Multi-axis Controller
  #define BUTTON_COUNT 2  // Indicates how many buttons will be available on the joystick. (0-32)
  #define HAT_SWITCH_COUNT 0  // Indicates how many hat switches will be available on the joystick. (0-2)
  #define INCLUDE_X_AXIS true  // Indicates if the X Axis is available on the joystick.
  #define INCLUDE_Y_AXIS true  // Indicates if the Y Axis is available on the joystick.
  #define INCLUDE_Z_AXIS true  // Indicates if the Z Axis (in some situations this is the right X Axis) is available on the joystick.
  #define INCLUDE_RX_AXIS true  // Indicates if the X Axis Rotation (in some situations this is the right Y Axis) is available on the joystick.
  #define INCLUDE_RY_AXIS true  // Indicates if the Y Axis Rotation is available on the joystick.
  #define INCLUDE_RZ_AXIS true  // Indicates if the Z Axis Rotation is available on the joystick.
  #define INCLUDE_RUDDER true  // Indicates if the Rudder is available on the joystick.
  #define INCLUDE_THROTTLE true  // Indicates if the Throttle is available on the joystick.
  #define INCLUDE_ACCELERATOR true  // Indicates if the Accelerator is available on the joystick.
  #define INCLUDE_BRAKE true  // Indicates if the Brake is available on the joystick.
  #define INCLUDE_STEERING true  // Indicates if the Steering is available on the joystick.

  // Joystick configuration (manual in src/ArduinoJoystickLibrary/README.md)
  Joystick_ Joystick(HID_REPORT_ID, JOYSTICK_TYPE, BUTTON_COUNT, HAT_SWITCH_COUNT,
                     INCLUDE_X_AXIS, INCLUDE_Y_AXIS, INCLUDE_Z_AXIS, INCLUDE_RX_AXIS,
                     INCLUDE_RY_AXIS, INCLUDE_RZ_AXIS, INCLUDE_RUDDER, INCLUDE_THROTTLE,
                     INCLUDE_ACCELERATOR, INCLUDE_BRAKE, INCLUDE_STEERING);

  // Initialize joystick axis ranges
  void initJoystick() {
    Joystick.setXAxisRange(rc_min_values[0], rc_max_values[0]);
    Joystick.setYAxisRange(rc_min_values[1], rc_max_values[1]);
    Joystick.setZAxisRange(rc_min_values[2], rc_max_values[2]);
    Joystick.setRzAxisRange(rc_min_values[3], rc_max_values[3]);
    Joystick.setRyAxisRange(rc_min_values[4], rc_max_values[4]);
    Joystick.setRxAxisRange(rc_min_values[5], rc_max_values[5]);
  }

  void startJoystick() {
    Joystick.begin();
  }
  
  // Send joystick values to usb host
  void outputJoystick() {
    static uint8_t localflags;
  
    if (tx_shared_flags) {  // If there is a new values to process
      localflags = tx_shared_flags;
  
      // Process every channel with a flag
      for (uint8_t i=0;i<CHANNELS;i++) {
        if (localflags & FLAGS[i]) {
          switch (i) {
            case 0:  // CH 1
              Joystick.setXAxis(rc_values[i]);
              break;
            case 1:  // CH 2
              Joystick.setYAxis(rc_values[i]);
              break;
            case 2:  // CH 3
              Joystick.setZAxis(rc_values[i]);
              break;
            case 3:  // CH 4
              Joystick.setRzAxis(rc_values[i]);
              break;
            case 4:  // CH 5
              Joystick.setRyAxis(rc_values[i]);
              break;
            case 5:  // CH 6
              Joystick.setRxAxis(rc_values[i]);
              break;

            #if CHANNELS > 6
              case 6:  // CH 7
                Joystick.setButton(0, rc_values[i] < STICK_CENTER ? 0 : 1);
                break;
              case 7:  // CH 8
                Joystick.setButton(1, rc_values[i] < STICK_CENTER ? 0 : 1);
                break;
            #endif
          }
        }
      }
  
      tx_shared_flags = 0;  // Reset flags
    }
  
    localflags = 0;
  }
  
#endif
