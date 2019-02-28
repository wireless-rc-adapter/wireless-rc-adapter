/* Arduino Wireless RC Adapter
 * 
 * Connects a PWM receiver as a HID compatible
 * game controller to almost any kind of device.
 * 
 * GregNau    2016
 */
#include <EEPROM.h>
#include <PinChangeInterrupt.h>
#include <Joystick.h>

//#define DEBUG_ENABLED
//#define PWM_RECEIVER
#define PPM_RECEIVER

uint16_t rc_values[6] = {0, 0, 0, 0, 0, 0};  // Actual channel values
uint16_t rc_min_values[6],rc_max_values[6];  // Calibration data

#ifdef PWM_RECEIVER

uint8_t rc_pins[6] = {8, 9, 10, 16, 14, 15};  // Receiver input pins
uint8_t rc_flags[6] = {1, 2, 4, 8, 16, 32};

volatile uint8_t rc_shared_flags;
volatile uint16_t rc_shared_values[6];
volatile uint32_t rc_shared_ts[6];

#endif

unsigned long calTimer,ledTimer;
boolean calMode;

Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID, 
  JOYSTICK_TYPE_JOYSTICK, 2, 0,
  true, true, true, false, false, true,
  false, false, false, false, false);

void setup() {

  Joystick.setXAxisRange(-127, 127);
  Joystick.setYAxisRange(-127, 127);
  Joystick.setZAxisRange(-127, 127);
  Joystick.setRzAxisRange(-127, 127);

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
  
  #ifdef PWM_RECEIVER
  rc_setup_interrupts(); // Attach interrupt timers to receiver pins
  #endif
  #ifdef PPM_RECEIVER
  rc_setup_ppm();
  #endif
  
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
  #ifdef PWM_RECEIVER
  rc_process_channels();  // Measure channels pwm timing values.
  #endif
  
  #ifdef DEBUG_ENABLED
  if (!calMode) {
    rc_print_channels();  // Print channel values on serial terminal.
  }
  else calibration();  // Do calibration if flag active.
  #else
  if (!calMode) {  // Joystick functions
    // CH 1 (PIN 8)
    Joystick.setZAxis(map(rc_values[0],rc_min_values[0],rc_max_values[0], -127, 127));
    // CH 2 (PIN 9)
    Joystick.setRzAxis(map(rc_values[1],rc_min_values[1],rc_max_values[1], -127, 127));
    // CH 3 (PIN 10)
    Joystick.setYAxis(map(rc_values[2],rc_min_values[2],rc_max_values[2], -127, 127));
    // CH 4 (PIN 16)
    Joystick.setXAxis(map(rc_values[3],rc_min_values[3],rc_max_values[3], -127, 127));
    // CH 5 (PIN 14)
    Joystick.setButton(0, rc_values[4] < 1500 ? 0 : 1);
    // CH 6 (PIN 15)
    Joystick.setButton(1, rc_values[5] < 1500 ? 0 : 1);
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
