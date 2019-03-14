#if defined(SERIAL_DEBUG)

  #if !defined(SERIAL_SPD)
    #define SERIAL_SPD 9600
  #endif

  uint32_t serial_timer;
  
  void initSerial() {
    Serial.begin(SERIAL_SPD);
  
    #if defined(ARDUINO_AVR_PROMICRO) || defined(ARDUINO_AVR_MICRO) || defined(ARDUINO_AVR_LEONARDO)
      while (!Serial) ledOn();  // Wait here for an active serial connection
    #endif
  
    Serial.println("Wireless RC Adapter (DEBUG MODE)\n");
  }

  void rcPrintChannels() {
    uint32_t nowtime = millis();

    if (nowtime >= serial_timer) {
      static char str[CHANNELS*13] = {0};  // Must be defined as static
      str[0] = (char)0;  // Therefore to make sure it is clean
      
      for (uint8_t b=0;b<CHANNELS;b++) {
        sprintf(str, "%sCH%d: %d | ", str, b+1, rc_values[b]);
      }
    
      Serial.print(str);
      Serial.println();

      serial_timer = nowtime + 50;
    }
  }
#endif
