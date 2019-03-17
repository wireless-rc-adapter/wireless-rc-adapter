
// ToDo - replace compare values with STICK_CENTER THRESHOLD ETC

// Set default timeout if not configured
#if !defined(CAL_TIMEOUT)
  #define CAL_TIMEOUT 5000
#endif

// Set default calibration trigger cannel if not configured
#if !defined(CAL_CHANNEL)
  #if CHANNELS > 2
    #define CAL_CHANNEL 3  // To CH3 when possible
  #else
    #define CAL_CHANNEL 1  // Otherwise to CH1
  #endif
#endif

#if defined(CAL_DISABLED)
  void rcCalibrate() {
    for (uint8_t e=0;e<CHANNELS;e++) {
      rc_min_values[e] = STICK_CENTER - STICK_HALFWAY;
      rc_max_values[e] = STICK_CENTER + STICK_HALFWAY;
    }
  }
#else
  bool cal_mode = false;
  uint32_t cal_timer = 0L;
  
  // Function to check and get calibration values
  void rcCalibrate() {
    cal_mode = checkIfCal();  // Check if calibration necessary
  
    #if defined(SERIAL_DEBUG)
      if (cal_mode) Serial.println("CALIBRATION ACTIVE");
    #endif
  
    // Set initial values off-range for calibration
    for (uint8_t i=0;i<CHANNELS;i++) {
      rc_min_values[i] = 2500;
      rc_max_values[i] = 0;
    }
  
    while (cal_mode) {
      #if defined(PWM_RECEIVER)
        rcProcessPwm();
      #endif
      
      unsigned long curtime = millis();  // Make a record of the current time
    
      waveLed(500);
    
      // ToDo - only process here ch's with new flag, like the process function of pwm
      for (uint8_t i=0;i<CHANNELS;i++) {
        if (rc_values[i] < 2500 && rc_values[i] > rc_max_values[i]) {  // ...looking for the MAX's,
          rc_max_values[i] = rc_values[i];
          cal_timer = curtime + CAL_TIMEOUT;
        }
        else if (rc_values[i] > 0 && rc_values[i] < rc_min_values[i]) {  // ...then for the MIN's
          rc_min_values[i] = rc_values[i];
          cal_timer = curtime + CAL_TIMEOUT;
        }
      }
  
      if (cal_timer && cal_timer <= curtime) {
        uint16_t diffvalues[CHANNELS];
        boolean diffresult = true;
  
        #if defined(SERIAL_DEBUG)
  //        calPrintValues();
  static char str[64];
  
        Serial.println("\tCH1\tCH2\tCH3\tCH4\tCH5\tCH6");
        sprintf(str,"MAX:\t%d\t%d\t%d\t%d\t%d\t%d\n",rc_max_values[0],rc_max_values[1],rc_max_values[2],rc_max_values[3],rc_max_values[4],rc_max_values[5]);
        Serial.print(str);
        sprintf(str,"MIN:\t%d\t%d\t%d\t%d\t%d\t%d\n",rc_min_values[0],rc_min_values[1],rc_min_values[2],rc_min_values[3],rc_min_values[4],rc_min_values[5]);
        Serial.print(str);
        #endif
  
        for (uint8_t x=0;x<CHANNELS;x++) {
          // Calculate min-max differences, to see...
          diffvalues[x] = rc_max_values[x] - rc_min_values[x];
          
          // ... whether it is "enough" or not
          if (diffvalues[x] < 360) {
            diffvalues[x] = 0;
            diffresult = false;
            
            #if defined(SERIAL_DEBUG)
              Serial.print("\t X ");
            #endif
          }
          #if defined(SERIAL_DEBUG)
            else {
              Serial.print("\tOK ");
            }
          #endif
        }
  
        if (diffresult) {
          blinkLed(4, 60);  // Led blinks when calibration succeeded
          writeMem();  // Store calibration values in the eeprom
  
          #if defined(SERIAL_DEBUG)
            Serial.print("\n\nCALIBRATION FINISHED\n");
          #endif
          cal_mode = false;  // Reset the flag
        }
        else {
          #if defined(SERIAL_DEBUG)
            Serial.print("\n\nALL CHANNEL MUST BE \"OK\" TO AUTOSAVE!\nOTHERWISE PRESS BUTTON TO SAVE CAL.DATA.\n\n");
          #endif
  
          cal_timer = 0;  // Reset timeout
        }
      }
    }
  }
  
  bool checkIfCal() {
    #if defined(SERIAL_DEBUG)
      uint32_t nexttime = millis();
    #endif
    
    // Wait here until valid signal on CAL_CHANNEL pin
    while (!rc_values[CAL_CHANNEL-1] && rc_values[CAL_CHANNEL-1] < 360) {
      #if defined(SERIAL_DEBUG)
        uint32_t  curtime = millis();
        
        if (curtime >= nexttime) {
            Serial.println("NO SIGNAL ON CAL_CHANNEL!");
          
          nexttime = curtime + 1000;
        }
      #endif
    }
    
    // Check if calibration necessary or triggered with full CAL_CHANNEL
    for (uint8_t d=0;d<CHANNELS;d++) {
      if (rc_min_values[d] < 360 || rc_max_values[d] > 2500
          || rc_min_values[d] > 1500 || rc_max_values[d] < 1500) {
        
        return true;
      }
    }
    if (rc_values[2] >= 1600) {
      
      return true;
    }
    else {
      #if defined(SERIAL_DEBUG)
        calPrintValues();
        Serial.println("CALIBRATION DATA LOADED FROM EEPROM.");
      #endif
      
      return false;
    }
  }
  
  #if defined(SERIAL_DEBUG)
    void calPrintValues() {
      static char str[CHANNELS*7+2];
      
      for (uint8_t c=0;c<CHANNELS;c++) {
        sprintf(str, "%s\t%d", str, rc_max_values[c]);
      }
    
      Serial.print("MAX:");
      Serial.print(str);
      Serial.println();

      str[0] = (char)0;  // Clean the char[array]
    
      for (uint8_t c=0;c<CHANNELS;c++) {
        sprintf(str, "%s\t%d", str, rc_min_values[c]);
      }
    
      Serial.print("MIN:");
      Serial.print(str);
      Serial.println();
        
    }
  #endif
#endif
