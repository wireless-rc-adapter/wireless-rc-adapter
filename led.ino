void initLed() {
  #if defined ARDUINO_AVR_PROMICRO
    TXLED0;
    RXLED0;
  #elif defined ARDUINO_AVR_MICRO || defined ARDUINO_AVR_LEONARDO
    #define RXLED 17  // RXLED is on pin 17
    #define TXLED 30  // TXLED is on pin 30

    pinMode(RXLED, OUTPUT);
    pinMode(TXLED, OUTPUT);
    digitalWrite(TXLED, LOW);
    digitalWrite(RXLED, LOW);
  #elif defined ARDUINO_AVR_NANO || defined ARDUINO_AVR_UNO || defined ARDUINO_AVR_DUEMILANOVE
    #define LEDPIN 13
    pinMode(LEDPIN, OUTPUT);
    digitalWrite(LEDPIN, LOW);
  #elif defined ARDUINO_AVR_ADK || defined ARDUINO_AVR_MEGA || defined ARDUINO_AVR_MEGA2560
    #define LEDPIN
    ;  // ToDo - support on MEGA ADK/2560 boards
  #endif
}

// Turns the led(s) on for various boards
void ledOn() {
  #if defined ARDUINO_AVR_PROMICRO
    TXLED1;
    RXLED1;
  #elif defined ARDUINO_AVR_MICRO || defined ARDUINO_AVR_LEONARDO
    digitalWrite(TXLED, HIGH);
    digitalWrite(RXLED, HIGH);
  #elif defined ARDUINO_AVR_NANO || defined ARDUINO_AVR_UNO || defined ARDUINO_AVR_DUEMILANOVE
    digitalWrite(LEDPIN, HIGH);
  #elif defined ARDUINO_AVR_ADK || defined ARDUINO_AVR_MEGA || defined ARDUINO_AVR_MEGA2560
    #define LEDPIN
    ;  // ToDo - support on MEGA ADK/2560 boards
  #endif
}

// Turns the led(s) off for various boards
void ledOff() {
  #if defined ARDUINO_AVR_PROMICRO
    TXLED0;
    RXLED0;
  #elif defined ARDUINO_AVR_MICRO || defined ARDUINO_AVR_LEONARDO
    digitalWrite(TXLED, LOW);
    digitalWrite(RXLED, LOW);
  #elif defined ARDUINO_AVR_NANO || defined ARDUINO_AVR_UNO || defined ARDUINO_AVR_DUEMILANOVE
    digitalWrite(LEDPIN, LOW);
  #elif defined ARDUINO_AVR_ADK || defined ARDUINO_AVR_MEGA || defined ARDUINO_AVR_MEGA2560
    #define LEDPIN
    ;  // ToDo - support on MEGA ADK/2560 boards
  #endif
}

// It is a blocking function on purpose, do not use it
void blinkLed(uint8_t times, uint8_t dly) {
  #if defined ARDUINO_AVR_PROMICRO
    for (uint8_t t=0;t<times;t++) {
        TXLED1;
        RXLED1;
        delay(dly);
        TXLED0;
        RXLED0;
        delay(dly);
    }
  #elif defined ARDUINO_AVR_MICRO || defined ARDUINO_AVR_LEONARDO
    #define RXLED 17  // RXLED is on pin 17
    #define TXLED 30  // TXLED is on pin 30

    for (uint8_t t=0;t<times;t++) {
      digitalWrite(TXLED, HIGH);
      digitalWrite(RXLED, HIGH);
      delay(dly);
      digitalWrite(TXLED, LOW);
      digitalWrite(RXLED, LOW);
      delay(dly);
    }
  #elif defined ARDUINO_AVR_NANO || defined ARDUINO_AVR_UNO || defined ARDUINO_AVR_DUEMILANOVE
    #define LEDPIN 13  // Led "L" is on pin D13

    for (uint8_t t=0;t<times;t++) {
      digitalWrite(LEDPIN, HIGH);
      delay(dly);
      digitalWrite(LEDPIN, LOW);
      delay(dly);
    }
  #elif defined ARDUINO_AVR_ADK || defined ARDUINO_AVR_MEGA || defined ARDUINO_AVR_MEGA2560
    ;  // ToDo - support on MEGA ADK/2560 boards
  #endif
}

void waveLed(uint16_t dly) {
  uint32_t curtime = millis();
  
  #if defined ARDUINO_AVR_PROMICRO
    if (curtime - led_timer >= dly) {
      if (!led_mode) {
        TXLED1;
        RXLED0;
      }
      else {
        TXLED0;
        RXLED1;
      }
      led_mode = !led_mode;
      led_timer = curtime;
    }
  #elif defined ARDUINO_AVR_MICRO || defined ARDUINO_AVR_LEONARDO
    if (curtime - led_timer >= dly) {
      if (!led_mode) {
        digitalWrite(TXLED, !led_mode);
        digitalWrite(RXLED, led_mode);
      }
      else {
        digitalWrite(TXLED, led_mode);
        digitalWrite(RXLED, !led_mode);
      }
      led_mode = !led_mode;
      led_timer = curtime;
    }
  #elif defined ARDUINO_AVR_NANO || defined ARDUINO_AVR_UNO || defined ARDUINO_AVR_DUEMILANOVE
    if (curtime - led_timer >= dly) {
      if (!led_mode) {
        digitalWrite(LEDPIN, led_mode);
      }
      else {
        digitalWrite(LEDPIN, led_mode);
      }
      led_mode = !led_mode;
      led_timer = curtime;
    }
  #endif
}
