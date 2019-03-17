/* Wireless RC Adapter (DEVELOPER VERSION)
 *  http://wireless-rc-adapter.github.io
 * 
 * Connects an RC receiver as a HID compatible
 * joystick to almost any kind of device, with
 * just a simple USB cable.
 * 
 *  GregNau    2015-2019
 */


// Configuration options
//  (Wiki: http://github.com/wireless-rc-adapter/wireless-rc-adapter/wiki)

// >>> Input signal modulation (uncomment only one!) <<<
//#define PWM_RECEIVER  // Enable Pulse Width Modulation receiver
//#define PPM_RECEIVER  // Enable Pulse Position Modulation receiver

//#define CHANNELS 4  // Override the default 6 channels (PPM max: 8, PWM max: 6)

// >>> Serial-Debug options for troubleshooting <<<
//#define SERIAL_DEBUG  // Enable Serial Debug by uncommenting this line
//#define SERIAL_SPD 115200  // Set debug bitrate between 9600-115200 bps (default: 9600)

// >>> Calibration settings <<<
//#define CAL_TIMEOUT  8000  // Set auto-accept timout in ms for calibration (default: 5000)
//#define CAL_CHANNEL  3  // Set which channel can trigger calibration on boot (default: 3 or 1)
//#define CAL_DISABLE  // Uncommenting this line causes the calibration to be disabled *
// (* when calibration disabled, do not forget to set stick center and halfway below!)

// >>> Joystick settings, trim, etc <<<
//#define STICK_CENTER 1500  // Configure custom stick center value (disables calibration! | default: 1500)
//#define STICK_HALFWAY 500  // Define custom halfway value (disables calibration! | default: 500)
//#define THRESHOLD 100  // Customize channel value detection threshold (disables calibration! | default: 100)
//#define FUTABA  // Futaba PPM fix (disables calibration!)
//#define COMPAT_FIX  // In case of joystick issues, try to enable this option. (eg. Aerofly sim)

// End of Configuration options


/**************** DO NOT EDIT BELOW THIS LINE, UNLESS YOU KNOW HOW TO FIX IT ****************/


// Check if board is compatible and set board specific options
#if defined(ARDUINO_AVR_NANO) || defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_DUEMILANOVE) || defined(ARDUINO_AVR_ADK) || defined(ARDUINO_AVR_MEGA) || defined(ARDUINO_AVR_MEGA2560)
  // Arduino Nano, Uno, Duemilanove, Mega, 2560, ADK
  #define SERIAL_DEBUG
#elif defined(ARDUINO_AVR_PROMICRO) || defined(ARDUINO_AVR_LEONARDO) || defined(ARDUINO_AVR_MICRO)
  // Sparkfun ProMicro, Arduino Leonardo & Micro
#else
  #error Not compatible board selected. For a list of compatible boards please see the Wiki pages.
#endif

// Error checking for receiver modulation configuration
#if defined(PWM_RECEIVER) && defined(PPM_RECEIVER)
  #error Both PWM_ and PPM_ receiver is enabled, please disable one!
#elif !defined(PWM_RECEIVER) && !defined(PPM_RECEIVER)
  #error No _RECEIVER modulation have been configured, please enable one!
#endif

// Error checking for channels configuration
#if !defined(CHANNELS)
  #define CHANNELS 6  // Set default 6 channels
#elif CHANNELS > 8
  #error Too much channels are configured, the maximum is 8 for now!
#endif

// Configure joystick settings
#if defined(FUTABA)
  #define CAL_DISABLE  // Disable calibration if custom center and halfway settings are configured
  #if !defined(STICK_CENTER)
    #define STICK_CENTER 1530
  #endif
  #if !defined(STICK_HALFWAY)
    #define STICK_HALFWAY 450
  #endif
  #if !defined(THRESHOLD)
    #define THRESHOLD 200  // Threshold is used to detect PPM values (added to range at both ends)
  #endif
#else
  #if !defined(STICK_CENTER)
    #define STICK_CENTER 1500
  #else
    #define CAL_DISABLE
  #endif
  #if !defined(STICK_HALFWAY)
    #define STICK_HALFWAY 500
  #else
    #define CAL_DISABLE
  #endif
  #if !defined(THRESHOLD)
    #define THRESHOLD 100  // Threshold is used to detect PPM values (added to range at both ends)
  #else
    #define CAL_DISABLE
  #endif
#endif

// Global variables
const uint8_t FLAGS[8] = {1, 2, 4, 8, 16, 32, 64, 128};  // Channel value position flags
volatile uint16_t rc_values[CHANNELS] = {0};  // Actual channel values
uint16_t rc_min_values[CHANNELS], rc_max_values[CHANNELS];  // Calibration data arrays
uint8_t tx_shared_flags = 0;

// Setup function
void setup() {
  initLed();  // Configure and init the leds on the board
  readMem();  // Read calibration data from eeprom

  #if defined(SERIAL_DEBUG)
    initSerial();  // Start serial debug output
  #endif

  #if defined(PPM_RECEIVER)
    rcSetupPpm();  // Attach interrupt timer to PPM pin
  #elif defined(PWM_RECEIVER)
    rcSetupPwm();  // Attach interrupt timers to PWM pins
  #endif

  delay(250);  // Give signals some time to settle...

  // Waits here until valid signal on CAL_CHANNEL
  rcCalibrate();  // Check if calibration necessary or triggered with full throttle

  #if !defined(SERIAL_DEBUG)
    initJoystick();  // Setup joystick output
    startJoystick();  // Start joystick output
  #endif
}

void loop() {
  #if defined(PWM_RECEIVER)
    rcProcessPwm();  // Measure channels pwm timing values.
  #endif
  
  #if defined(SERIAL_DEBUG)
    rcPrintChannels();  // Print RAW channel values on serial terminal.
  #else
    outputJoystick();  // Output channels where there is new data
  #endif
}
