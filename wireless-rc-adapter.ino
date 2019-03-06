/* Wireless RC Adapter
 * 
 * Connects an RC receiver as a HID compatible
 * joystick to almost any kind of device, with
 * just a simple USB cable.
 * 
 * GregNau    2016-2019
 */

// Configuration options
//#define PWM_RECEIVER  // Enable PWM receiver (!only if PPM disabled!)
#define PPM_RECEIVER  // Enable PPM receiver (!only if PWM disabled!)
//#define DEBUG_ENABLED  // Enable Serial Debug
//#define DEBUG_SERSPD 115200  // Serial Debug bps (9600-115200)
#define CAL_TIMEOUT 7000  // Set auto-accept timout in ms for calibration
//#define FUTABA  // Futaba PPM workaround


/**************** DO NOT EDIT BELOW THIS LINE, UNLESS YOU KNOW HOW TO FIX IT ****************/


// Global variables
bool cal_mode, led_mode;
uint8_t tx_shared_flags;
uint32_t cal_timer, led_timer;
uint16_t rc_values[6] = {0, 0, 0, 0, 0, 0};  // Actual channel values
uint16_t rc_min_values[6], rc_max_values[6];  // Calibration data arrays
const uint8_t FLAGS[6] = {1, 2, 4, 8, 16, 32};  // Channel position flags

#if defined PWM_RECEIVER
  volatile uint8_t rc_shared_flags; // Receiver shared flags byte
  volatile uint32_t rc_shared_ts[6];  // Long-array to store TimeSequences
  volatile uint16_t rc_shared_values[6];  // Shared receiver values int-array

  #if defined ARDUINO_AVR_PROMICRO
    const uint8_t RC_PINS[6] = {8, 9, 10, 16, 14, 15};  // Input pins on SparkFun ProMicro
  #elif defined ARDUINO_AVR_MICRO || defined ARDUINO_AVR_LEONARDO
    const uint8_t RC_PINS[4] = {8, 9, 10, 11};  // Input pins on Arduino Leonardo/Micro
  #elif defined ARDUINO_AVR_NANO || defined ARDUINO_AVR_UNO || defined ARDUINO_AVR_DUEMILANOVE  // No joystick...
    #define DEBUG_ENABLED  // Only serial debug
    const uint8_t RC_PINS[6] = {A0, A1, A2, A3, A4, A5};  // Input pins on Arduino Nano,Uno,Due
  #elif defined ARDUINO_AVR_ADK || defined ARDUINO_AVR_MEGA || defined ARDUINO_AVR_MEGA2560  // No Joystick...
    #define DEBUG_ENABLED  // Only serial debug
    const uint8_t RC_PINS[6] = {A10, A11, A12, A13, A14, A15};  // Input pins on Arduino Mega, ADK, 2560
  #endif
#elif defined PPM_RECEIVER
  #if defined ARDUINO_AVR_NANO || defined ARDUINO_AVR_UNO || defined ARDUINO_AVR_DUEMILANOVE || defined ARDUINO_AVR_ADK || defined ARDUINO_AVR_MEGA || defined ARDUINO_AVR_MEGA2560
    #define DEBUG_ENABLED  // Only serial debug, no joystick!
  #endif
#endif

#if !defined DEBUG_ENABLED
  #include <Joystick.h>
  
  Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID,
                     JOYSTICK_TYPE_JOYSTICK, 2, 0,
                     true, true, true, false, false, true,
                     false, false, false, false, false);
#endif

void setup() {
  initButton();
  initLed();

  #if defined DEBUG_ENABLED
    initSerial();
  #else
    initJoystick();
  #endif
  
  readMem();  // Read calibration data from eeprom

  #if defined PPM_RECEIVER
    rc_setup_ppm();  // Attach interrupt timer to ppm pin
  #elif defined PWM_RECEIVER
    rc_setup_pwm();  // Attach interrupt timers to pwm pins
  #endif
  
  checkIfCal();  // Check calibration values and enter cal.mode if necessary
}

void loop() {
  #if defined PWM_RECEIVER
    pwm_process_channels();  // Measure channels pwm timing values.
  #endif
  
  #if defined DEBUG_ENABLED
    if (!cal_mode) rc_print_channels();  // Print channel values on serial terminal.
    else calibration();  // Do calibration if flag active.
  #else
    if (!cal_mode) outputJoystick();  // Joystick functions
    else calibration();  // Do calibration if flag active
  #endif
}
