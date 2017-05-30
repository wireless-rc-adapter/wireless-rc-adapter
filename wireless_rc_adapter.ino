/* Arduino Wireless RC Adapter
 * 
 * Connects a PWM receiver as a HID compatible
 * game controller to almost any kind of device.
 * 
 * GregNau    2016
 */
#include <EEPROM.h>
#include <PinChangeInterrupt.h>

//#define DEBUG_ENABLED

uint8_t rc_pins[6] = {8, 9, 10, 16, 14, 15};  // Receiver input pins
uint8_t rc_flags[6] = {1, 2, 4, 8, 16, 32};
uint16_t rc_values[6] = {0, 0, 0, 0, 0, 0};  // Actual channel values
uint16_t rc_min_values[6],rc_max_values[6];  // Calibration data

volatile uint8_t rc_shared_flags;
volatile uint16_t rc_shared_values[6];
volatile uint32_t rc_shared_ts[6];

unsigned long calTimer,ledTimer;
boolean calMode;

void setup() {
  pinMode(A1, INPUT_PULLUP);  // Initialize the button pin
  
  #ifdef DEBUG_ENABLED
    Serial.begin(9600);
    while (!Serial) {  // Wait here for a serial connection
      TXLED1;
      RXLED0;
      delay(250);
      TXLED0;
      RXLED1;
      delay(250);
    }
    Serial.println("Arduino Wireless RC Adapter (DEBUG MODE)\n");
  #endif
  
  readMem();  // Read calibration data from eeprom
  
  rc_setup_interrupts(); // Attach interrupt timers to receiver pins
  
  // Check calibration values and enter cal.mode if necessary
  if (rc_min_values[0,1,2,3,4,5] < 360 || rc_max_values[0,1,2,3,4,5] > 3000 || digitalRead(A1) == LOW) {
    calMode = true;

    // Turn leds on during calibration
    TXLED1;
    RXLED1;

    #ifdef DEBUG_ENABLED
      Serial.println("CALIBRATION ACTIVE");
    #endif
    
    // Set initial minimum values (very?) high for calibration
    for (byte i=0;i<6;i++) {
        rc_min_values[i] = 2500;
        rc_max_values[i] = 0;
    }
  }
  else {
    #ifdef DEBUG_ENABLED
      Serial.println("CALIBRATION DATA LOADED FROM EEPROM.");
    #else
      Joystick.begin();  // init joystick with 'autosendvalues' enabled
    #endif
  }
}

void loop() {
  rc_process_channels();  // Measure channels pwm timing values.
  
  #ifdef DEBUG_ENABLED
  if (!calMode) {
    rc_print_channels();  // Print channel values on serial terminal.
  }
  else calibration();  // Do calibration if flag active.
  #else
  if (!calMode) {  // Joystick functions
    // CH 1 (PIN 8)
    Joystick.setXAxis(map(rc_values[0],rc_min_values[0],rc_max_values[0], -127, 127));
    // CH 2 (PIN 9)
    Joystick.setYAxis(map(rc_values[1],rc_min_values[1],rc_max_values[1], -127, 127));
    // CH 3 (PIN 10)
    Joystick.setZAxis(map(rc_values[2],rc_min_values[2],rc_max_values[2], -127, 127));
    // CH 4 (PIN 16)
    Joystick.setXAxisRotation(map(rc_values[3],rc_min_values[3],rc_max_values[3], 0, 360));
    // CH 5 (PIN 14)
    Joystick.setThrottle(map(rc_values[4],rc_min_values[4],rc_max_values[4], 0, 255));
    // CH 6 (PIN 15)
    Joystick.setRudder(map(rc_values[5],rc_min_values[5],rc_max_values[5], 0, 255));
  }
  else calibration();  // Do calibration if flag active.
  #endif
}

#ifdef DEBUG_ENABLED
void rc_print_channels() {
  static char str[70];

  sprintf(str, "CH1: %d | CH2: %d | CH3: %d | CH4: %d | CH5: %d | CH6: %d\n",
    rc_values[0], rc_values[1], rc_values[2], rc_values[3], rc_values[4], rc_values[5]);

  Serial.print(str);
}
#endif
