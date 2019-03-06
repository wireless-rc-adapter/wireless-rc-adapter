#if defined PWM_RECEIVER
  #include <PinChangeInterrupt.h>
  
  void rc_channel_change(uint8_t id) {
    if (digitalRead(RC_PINS[id]) == HIGH) {
      rc_shared_ts[id] = micros();
    }
    else {
      rc_shared_values[id] = (uint16_t)(micros() - rc_shared_ts[id]);
      rc_shared_flags |= FLAGS[id];
    }
  }
  
  void rc_ch1_change() { rc_channel_change(0); }
  void rc_ch2_change() { rc_channel_change(1); }
  void rc_ch3_change() { rc_channel_change(2); }
  void rc_ch4_change() { rc_channel_change(3); }
  void rc_ch5_change() { rc_channel_change(4); }
  void rc_ch6_change() { rc_channel_change(5); }
  
  void rc_setup_pwm() {
    attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(RC_PINS[0]), rc_ch1_change, CHANGE);
    attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(RC_PINS[1]), rc_ch2_change, CHANGE);
    attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(RC_PINS[2]), rc_ch3_change, CHANGE);
    attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(RC_PINS[3]), rc_ch4_change, CHANGE);
  
    #if !defined ARDUINO_AVR_LEONARDO || !defined ARDUINO_AVR_MICRO
      attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(RC_PINS[4]), rc_ch5_change, CHANGE);
      attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(RC_PINS[5]), rc_ch6_change, CHANGE);
    #endif
  }
  
  // Process received channel values
  void pwm_process_channels() {
    static uint8_t localflags;
    
    if (rc_shared_flags) {  // If there is a new values to process
      noInterrupts();  // Turn off interrupts
      localflags = rc_shared_flags;
  
      // Process every channel with a set flag
      for (uint8_t i=0;i<sizeof(FLAGS);i++) {
        if (localflags & FLAGS[i]) {
          rc_values[i] = rc_shared_values[i];
          tx_shared_flags |= FLAGS[i];  // Set the 'new data to process' flag
        }
      }
      rc_shared_flags = 0;
      interrupts();
    }
  
    localflags = 0;
  }
#endif
