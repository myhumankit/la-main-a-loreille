#include <EEPROM.h>

void setup() {
  // No need to write the entire EEPROM:
  // Only the first byte is used to store the number of known tags.
  EEPROM.write(0x00, 0x00);
}

void loop() {}
