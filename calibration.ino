void calibration() {
  unsigned long curtime = millis();

  waveLed(500);

  // ToDo - only process here ch's with new flag, like the process function of pwm
  
  for (byte i=0;i<sizeof(FLAGS);i++) {
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
    uint16_t rc_diff_values[6];

  #if defined DEBUG_ENABLED
    static char str[64];

    Serial.println("\tCH1\tCH2\tCH3\tCH4\tCH5\tCH6");
    sprintf(str, "MAX:\t%d\t%d\t%d\t%d\t%d\t%d\n", rc_max_values[0], rc_max_values[1], rc_max_values[2], rc_max_values[3], rc_max_values[4], rc_max_values[5]);
    Serial.print(str);
    sprintf(str, "MIN:\t%d\t%d\t%d\t%d\t%d\t%d\n", rc_min_values[0], rc_min_values[1], rc_min_values[2], rc_min_values[3], rc_min_values[4], rc_min_values[5]);
    Serial.print(str);
  #endif

    for (byte x=0;x<sizeof(FLAGS);x++) {
      rc_diff_values[x] = rc_max_values[x] - rc_min_values[x];
      if (rc_diff_values[x] < 360) {
        rc_diff_values[x] = 0;
      #if defined DEBUG_ENABLED
        Serial.print("\t X ");
      #endif
      }
      #if defined DEBUG_ENABLED
        else {
          Serial.print("\tOK ");
        }
      #endif
    }

    if (rc_diff_values[0] && rc_diff_values[1] && rc_diff_values[2]
        && rc_diff_values[3] && rc_diff_values[4] && rc_diff_values[5]
        || digitalRead(A1) == LOW) {
      blinkLed(4, 60);  // Led blinks when calibration succeeded
      writeMem();  // Store cal.values in the eeprom

      #if defined DEBUG_ENABLED
        Serial.print("\n\nCALIBRATION FINISHED\n");
      #else
        Joystick.begin();
      #endif

      cal_mode = false;  // Reset the flag
    }
    else {
      #if defined DEBUG_ENABLED
        Serial.print("\n\nALL CHANNEL MUST BE \"OK\" TO AUTOSAVE!\nOTHERWISE PRESS BUTTON TO SAVE CAL.DATA.\n\n");
      #endif

      cal_timer = 0;  // Reset timeout
    }
  }
}

void checkIfCal() {
  if (rc_min_values[0, 1, 2, 3, 4, 5] < 360
      || rc_max_values[0, 1, 2, 3, 4, 5] > 3000
      || digitalRead(A1) == LOW) {
    cal_mode = true;

    #if defined DEBUG_ENABLED
      Serial.println("CALIBRATION ACTIVE");
    #endif

    // Turn leds on before calibration
    ledOn();

    // Set initial minimum values too high for triggering calibration
    for (uint8_t i=0;i<sizeof(FLAGS);i++) {
      rc_min_values[i] = 2500;
      rc_max_values[i] = 0;
    }
  }
  else {
    #if defined DEBUG_ENABLED
      Serial.println("CALIBRATION DATA LOADED FROM EEPROM.");
    #else
      Joystick.begin();  // Init joystick with 'autosendvalues' enabled
    #endif
  }
}

void initButton() {
  pinMode(A1, INPUT);  // Initialize the button pin
  digitalWrite(A1, HIGH);  // Activate the internat pullup-resistor
}
