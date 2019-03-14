#if !defined(SERIAL_DEBUG)
  #include "src/ArduinoJoystickLibrary/Joystick.h"

  // Joystick configuration (manual in src/ArduinoJoystickLibrary/README.md)
  Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID,
                     JOYSTICK_TYPE_JOYSTICK, 2, 0,
                     true, true, true, false, false, true,
                     false, false, false, false, false);

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
