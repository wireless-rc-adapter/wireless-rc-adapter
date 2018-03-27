/*
 * Added by Bayu Laksono
 * https://github.com/sblaksono/wireless_rc_adapter
 *
 * 2018-03-27
 *
 * Code to read PPM taken from : 
 * https://github.com/timonorawski/RCPPMJoystick
 * by Timon Orawski 
 * 2016-04-11  
 * Copyright (c) 2016, Timon Orawski
 */
#ifdef PPM_RECEIVER

#include <avr/interrupt.h>

//#define FUTABA

#ifdef FUTABA
#define STICK_HALFWAY 450
#define STICK_CENTER 1530
#define THRESHOLD 200
#else
#ifdef CUSTOM_STICK_CENTER
#define STICK_CENTER CUSTOM_STICK_CENTER
#else
#define STICK_CENTER 1500
#endif
#define STICK_HALFWAY 500
#define THRESHOLD 100 // threshold is used to detect PPM values (added to range at both ends)
#endif

#define USB_STICK_VALUE_MIN 0
#define USB_STICK_VALUE_MAX 1000

#define USB_STICK_ROTATION_VALUE_MIN 0
#define USB_STICK_ROTATION_VALUE_MAX 1000

#define MIN_PULSE_WIDTH (STICK_CENTER - STICK_HALFWAY - 15)
#define MAX_PULSE_WIDTH (STICK_CENTER + STICK_HALFWAY + 15)

#define RC_CHANNELS_COUNT 6

#define PPM_CAPTURE_PIN 4
#define NEWFRAME_PULSE_WIDTH 3000
#define TIMER_COUNT_DIVIDER 2

void rc_setup_ppm() {
  setupPins();
  initTimer();
}

void setupPins(void) {
  // Set up the Input Capture pin
  pinMode(PPM_CAPTURE_PIN, INPUT);
  digitalWrite(PPM_CAPTURE_PIN, 1); // enable the pullup
}

void initTimer(void) {
  // Input Capture setup
  // ICNC1: =0 Disable Input Capture Noise Canceler to prevent delay in reading
  // ICES1: =1 for trigger on rising edge
  // CS11: =1 set prescaler to 1/8 system clock (F_CPU)
  TCCR1A = 0;
  TCCR1B = (0 << ICNC1) | (1 << ICES1) | (1 << CS11);
  TCCR1C = 0;

  // Interrupt setup
  // ICIE1: Input capture
  TIFR1 = (1 << ICF1); // clear pending
  TIMSK1 = (1 << ICIE1); // and enable
}

uint16_t adjust(uint16_t diff, uint8_t chan) {
  // Here you can trim your rc values (e.g. if TX has no trims).

  // switch (chan) {
  //   case THROTTLE: return diff + 50;
  //   case YAW:      return diff + 60;
  //   case PITCH:    return diff + 60;
  //   case ROLL:     return diff + 90;
  //   case AUX1:     return diff + 10;
  // }

  //convert to microseconds (because of timer prescaler usage)
  return diff / TIMER_COUNT_DIVIDER;
}

ISR(TIMER1_CAPT_vect) {
  union twoBytes {
    uint16_t word;
    uint8_t  byte[2];
  } timeValue;

  uint16_t now, diff;
  static uint16_t last = 0;
  static uint8_t chan = 0;

  timeValue.byte[0] = ICR1L;    // grab captured timer value (low byte)
  timeValue.byte[1] = ICR1H;    // grab captured timer value (high byte)

  now = timeValue.word;
  diff = now - last;
  last = now;

  //all numbers are microseconds multiplied by TIMER_COUNT_DIVIDER (as prescaler is set to 1/8 of 16 MHz)
  if (diff > (NEWFRAME_PULSE_WIDTH * TIMER_COUNT_DIVIDER)) {
    chan = 0;  // new data frame detected, start again
  }
  else {
    if (diff > (MIN_PULSE_WIDTH * TIMER_COUNT_DIVIDER - THRESHOLD)
        && diff < (MAX_PULSE_WIDTH * TIMER_COUNT_DIVIDER + THRESHOLD)
        && chan < RC_CHANNELS_COUNT)
    {
      rc_values[chan] = adjust(diff, chan); //store detected value
    }
    chan++; //no value detected within expected range, move to next channel
  }
}

#endif
