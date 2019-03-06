#include <EEPROM.h>

void readMem() {
  uint8_t high,low;
  
  for (uint8_t i=0;i<sizeof(FLAGS);i++) {
    // Read the first 6 values (12 bytes) to an array
    high = EEPROM.read(i*2);
    low = EEPROM.read((i*2)+1);
    rc_min_values[i] = word(high,low);
    
    // Read the second 6 values (12 bytes) to an array
    high = EEPROM.read((i*2)+12);
    low = EEPROM.read((i*2)+13);
    rc_max_values[i] = word(high,low);
  }
}

void writeMem() {
  // Write min/max values low- and highBytes into EEPROM
  for (uint8_t i=0;i<sizeof(FLAGS);i++) {
    EEPROM.write(i*2,highByte(rc_min_values[i]));
    EEPROM.write((i*2)+1,lowByte(rc_min_values[i]));
    EEPROM.write((i*2)+12,highByte(rc_max_values[i]));
    EEPROM.write((i*2)+13,lowByte(rc_max_values[i]));
  }
}
