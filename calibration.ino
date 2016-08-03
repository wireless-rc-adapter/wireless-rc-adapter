void calibration() {
  #define TIMEOUT 5000  // Set timout(ms) for auto-accept calibration data
  unsigned long currentTime = millis();

  // Keep the leds on during calibration mode
  TXLED1;
  RXLED1;
  
  for (byte i=0;i<6;i++) {
    if (rc_values[i] < 2500 && rc_values[i] > rc_max_values[i]) {  // ...looking for the MAX's,
      rc_max_values[i] = rc_values[i];
      calTimer = currentTime + TIMEOUT;
    }
    else if (rc_values[i] > 0 && rc_values[i] < rc_min_values[i]) {  // ...then for the MIN's
      rc_min_values[i] = rc_values[i];
      calTimer = currentTime + TIMEOUT;
    }
  }
  
  if (calTimer && calTimer <= currentTime) {
    uint16_t rc_diff_values[6];
    
    #ifdef DEBUG_ENABLED
      static char str[64];

      Serial.println("\tCH1\tCH2\tCH3\tCH4\tCH5\tCH6");
      sprintf(str,"MAX:\t%d\t%d\t%d\t%d\t%d\t%d\n",rc_max_values[0],rc_max_values[1],rc_max_values[2],rc_max_values[3],rc_max_values[4],rc_max_values[5]);
      Serial.print(str);
      sprintf(str,"MIN:\t%d\t%d\t%d\t%d\t%d\t%d\n",rc_min_values[0],rc_min_values[1],rc_min_values[2],rc_min_values[3],rc_min_values[4],rc_min_values[5]);
      Serial.print(str);
    #endif
    
    for (byte x=0;x<6;x++) {
      rc_diff_values[x] = rc_max_values[x] - rc_min_values[x];
      if (rc_diff_values[x] < 360) {
        rc_diff_values[x] = 0;
        #ifdef DEBUG_ENABLED
          Serial.print("\t X ");
        #endif
      }
      #ifdef DEBUG_ENABLED
        else Serial.print("\tOK ");
      #endif
    }
    
    if (rc_diff_values[0] && rc_diff_values[1] && rc_diff_values[2] && rc_diff_values[3] && rc_diff_values[4] && rc_diff_values[5] || digitalRead(A1) == LOW) {
      ledBlink(4);  // Led blinks when calibration succeeded
      writeMem();  // Store cal.values in the eeprom
      
      #ifdef DEBUG_ENABLED
        Serial.print("\n\nCALIBRATION FINISHED\n");
      #else
        Joystick.begin();
      #endif
      
      calMode = false;  // reset the flag
    }
    else {
      #ifdef DEBUG_ENABLED
        Serial.print("\n\nALL CHANNEL MUST BE \"OK\" TO AUTOSAVE!\nOTHERWISE PRESS BUTTON TO SAVE CAL.DATA.\n\n");
      #endif
      
      calTimer = 0;  // Reset timeout
    }
  }
}

void ledBlink(byte n) {
  for (n; n>0; n--) {
    TXLED1;
    RXLED1;
    delay(80);
    TXLED0;
    RXLED0;
    delay(80);
  }
}
