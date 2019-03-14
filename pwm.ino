#if defined(PWM_RECEIVER)
  #include "src/PinChangeInterrupt/PinChangeInterrupt.h"

  #if defined(ARDUINO_AVR_PROMICRO)
    const uint8_t RC_PINS[6] = {8, 9, 10, 16, 14, 15};
  #elif defined(ARDUINO_AVR_MICRO) || defined(ARDUINO_AVR_LEONARDO)
    const uint8_t RC_PINS[6] = {8, 9, 10, 11, PB1, PB2};
  #elif defined(ARDUINO_AVR_NANO) || defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_DUEMILANOVE)
    const uint8_t RC_PINS[6] = {A1, A2, A3, A4, A5, A6};
  #elif defined(ARDUINO_AVR_ADK) || defined(ARDUINO_AVR_MEGA) || defined(ARDUINO_AVR_MEGA2560)
    const uint8_t RC_PINS[6] = {A10, A11, A12, A13, A14, A15};
  #endif

  volatile uint32_t rc_shared_ts[CHANNELS];  // Long-array to store TimeSequences
  volatile uint8_t rc_shared_flags; // Receiver shared flags byte
  volatile uint16_t rc_shared_values[CHANNELS];  // Shared receiver values int-array

  void rcChannelChange(uint8_t id) {
    if (digitalRead(RC_PINS[id]) == HIGH) {
      rc_shared_ts[id] = micros();
    }
    else {
      rc_shared_values[id] = (uint16_t)(micros() - rc_shared_ts[id]);
//      rc_shared_values[id] = constrain(rc_shared_values[id], rc_min_values[id], rc_max_values[id]);
      rc_shared_flags |= FLAGS[id];
    }
  }
  
  void rcCh1Change() { rcChannelChange(0); }
  void rcCh2Change() { rcChannelChange(1); }
  void rcCh3Change() { rcChannelChange(2); }
  void rcCh4Change() { rcChannelChange(3); }
  void rcCh5Change() { rcChannelChange(4); }
  void rcCh6Change() { rcChannelChange(5); }
  
  void rcSetupPwm() {
    #if CHANNELS > 0
      attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(RC_PINS[0]), rcCh1Change, CHANGE);
    #endif
    
    #if CHANNELS > 1
      attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(RC_PINS[1]), rcCh2Change, CHANGE);
    #endif

    #if CHANNELS > 2
      attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(RC_PINS[2]), rcCh3Change, CHANGE);
    #endif

    #if CHANNELS > 3
      attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(RC_PINS[3]), rcCh4Change, CHANGE);
    #endif

    #if CHANNELS > 4
      attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(RC_PINS[4]), rcCh5Change, CHANGE);
    #endif

    #if CHANNELS == 6
      attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(RC_PINS[5]), rcCh6Change, CHANGE);
    #else
      #error It is not yet possible to use more than 6 channels in PWM_ mode!
    #endif
  }

//  uint16_t adjust(uint16_t diff, uint8_t ch) {
//  // Here you can trim your rc values (e.g. if TX has no trims).
//
//  // switch (chan) {
//  //   case THROTTLE: return diff + 50;
//  //   case YAW:      return diff + 60;
//  //   case PITCH:    return diff + 60;
//  //   case ROLL:     return diff + 90;
//  //   case AUX1:     return diff + 10;
//  // }
//  
//  diff = diff / TIMER_COUNT_DIVIDER;
//  diff = constrain(diff, rc_min_values[chan], rc_max_values[chan]);
//  
//  return diff;
//}
  
  // Process received channel values
  void rcProcessPwm() {
    static uint8_t localflags;
    
    if (rc_shared_flags) {  // If there is a new values to process
      noInterrupts();  // Turn off interrupts
      localflags = rc_shared_flags;
  
      // Process every channel with a set flag
      for (uint8_t i=0;i<CHANNELS;i++) {
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
