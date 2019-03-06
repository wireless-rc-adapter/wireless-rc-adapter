#if defined DEBUG_ENABLED
  void initSerial() {
    #if defined DEBUG_SERSPD
      Serial.begin(DEBUG_SERSPD);
    #else
      Serial.begin(115200);
    #endif

    #if defined ARDUINO_AVR_PROMICRO || defined ARDUINO_AVR_MICRO || defined ARDUINO_AVR_LEONARDO
      while (!Serial) ledOn();  // Wait here for an active serial connection
    #endif
    Serial.println("Wireless RC Adapter (DEBUG MODE)\n");
  }
  
  void rc_print_channels() {
    static char str[70];
  
    sprintf(str, "CH1: %d | CH2: %d | CH3: %d | CH4: %d | CH5: %d | CH6: %d\n",
      rc_values[0], rc_values[1], rc_values[2], rc_values[3], rc_values[4], rc_values[5]);
  
    Serial.print(str);
  }
#endif
