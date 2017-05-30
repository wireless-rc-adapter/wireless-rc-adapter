void rc_channel_change(uint8_t id) {
  if (digitalRead(rc_pins[id]) == HIGH) {
    rc_shared_ts[id] = micros();
  }
  else {
    rc_shared_values[id] = (uint16_t)(micros() - rc_shared_ts[id]);
    rc_shared_flags |= rc_flags[id];
  }
}

void rc_ch1_change() { rc_channel_change(0); }
void rc_ch2_change() { rc_channel_change(1); }
void rc_ch3_change() { rc_channel_change(2); }
void rc_ch4_change() { rc_channel_change(3); }
void rc_ch5_change() { rc_channel_change(4); }
void rc_ch6_change() { rc_channel_change(5); }

void rc_setup_interrupts() {
  attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(rc_pins[0], rc_ch1_change, CHANGE);
  attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(rc_pins[1], rc_ch2_change, CHANGE);
  attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(rc_pins[2], rc_ch3_change, CHANGE);
  attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(rc_pins[3], rc_ch4_change, CHANGE);
  attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(rc_pins[4], rc_ch5_change, CHANGE);
  attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(rc_pins[5], rc_ch6_change, CHANGE);
}

void rc_process_channels() {
  static uint8_t flags;
  
  if (rc_shared_flags) {
    noInterrupts();
    flags = rc_shared_flags;
    for (byte i=0;i<sizeof(rc_flags);i++) {
      if (flags & rc_flags[i]) rc_values[i] = rc_shared_values[i];
    }
    rc_shared_flags = 0;
    interrupts(); 
  }

  flags = 0;
}
