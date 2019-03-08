#if !defined DEBUG_ENABLED
  void initJoystick() {
//    Joystick.setXAxisRange(-127, 127);
//    Joystick.setYAxisRange(-127, 127);
//    Joystick.setZAxisRange(-127, 127);
//    Joystick.setRzAxisRange(-127, 127);
    ;
  }
  
  void outputJoystick() {
    static uint8_t localflags;
  
    if (tx_shared_flags) {  // If there is a new values to process
      localflags = tx_shared_flags;
  
      // Process every channel with a flag
      for (uint8_t i=0;i<sizeof(FLAGS);i++) {
        if (localflags & FLAGS[i]) {
          switch (i) {
            case 0:  // CH 1
              Joystick.setXAxis(map(rc_values[0],rc_min_values[0],rc_max_values[0], 0, 1023));
              break;
            case 1:  // CH 2
              Joystick.setYAxis(map(rc_values[1],rc_min_values[1],rc_max_values[1], 0, 1023));
              break;
            case 2:  // CH 3
              Joystick.setZAxis(map(rc_values[2],rc_min_values[2],rc_max_values[2], 0, 1023));
              break;
            case 3:  // CH 4
              Joystick.setRzAxis(map(rc_values[3],rc_min_values[3],rc_max_values[3], 0, 1023));
              break;
            case 4:  // CH 5
              Joystick.setButton(0, rc_values[4] < 1500 ? 0 : 1);
              break;
            case 5:  // CH 6
              Joystick.setButton(1, rc_values[5] < 1500 ? 0 : 1);
              break;
          }
        }
      }
  
      tx_shared_flags = 0;  // Reset flags
    }
  
    localflags = 0;
  }
#endif
