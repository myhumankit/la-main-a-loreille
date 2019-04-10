// RFID Reader
// ************************************************************
#include <SPI.h>
#include <MFRC522.h>

// MFRC522_MOSI 11
// MFRC522_MISO 12
// MFRC522_SCK  13
#define MFRC522_RST_PIN 9
#define MFRC522_NSS_PIN 10

MFRC522 mfrc522(MFRC522_NSS_PIN, MFRC522_RST_PIN);  // Create MFRC522 instance

#define MAX_UID_SIZE 10
byte tagUID[MAX_UID_SIZE];

void resetCurrentTag() {
  // reset tagUID
  for (int i = 0; i < MAX_UID_SIZE; i++) {
    tagUID[i] = 0;
  }
}

uint8_t is_not_removed = 0;
byte lastTagNumber = 0;

// MP3 Player
// ************************************************************
#define DEFAULT_WAIT 200

/** Fonction de bas niveau pour envoyer une commande au module KT403A */
void mp3Command(byte command, byte data0, byte data1, unsigned long wait = DEFAULT_WAIT) {
  Serial.write(0x7e);    // starting byte
  Serial.write(0xff);    // version
  Serial.write(0x06);    // number of bytes of the command
  Serial.write(command); // command
  Serial.write(0x00);    // 0x00 no feedback, 0x01 feedback
  Serial.write(data0);
  Serial.write(data1);
  Serial.write(0xef);    // ending byte
  delay(wait);
}

void mp3Reset(unsigned long wait = DEFAULT_WAIT) {
  mp3Command(0x0c, 0x00, 0x00, wait);
}

void mp3SelectSDCard(unsigned long wait = DEFAULT_WAIT) {
  mp3Command(0x09, 0x00, 0x02, wait);
}

void mp3VolumeUp(unsigned long wait = DEFAULT_WAIT) {
  mp3Command(0x04, 0x00, 0x00, wait);
}

void mp3VolumeDown(unsigned long wait = DEFAULT_WAIT) {
  mp3Command(0x05, 0x00, 0x00, wait);
}

void mp3SetVolume(byte volume, unsigned long wait = DEFAULT_WAIT) {
  if (volume > 30) {
    volume = 30;
  }
  mp3Command(0x06, 0x00, volume, wait);
}

#define EQUALIZER_NORMAL  0x00
#define EQUALIZER_POP     0x01
#define EQUALIZER_ROCK    0x02
#define EQUALIZER_JAZZ    0x03
#define EQUALIZER_CLASSIC 0x04
#define EQUALIZER_BASS    0x05

void mp3SetEqualizer(byte mode, unsigned long wait = DEFAULT_WAIT) {
  mp3Command(0x07, 0x00, mode, wait);
}

void mp3PlaySong(byte folder, byte track, unsigned long wait = DEFAULT_WAIT) {
  mp3Command(0x0f, folder, track, wait);
}

void playResumeMusic(unsigned long wait = DEFAULT_WAIT) {
  mp3Command(0x0D, 0x00, 0x00, wait);
}

void pauseMusic(unsigned long wait = DEFAULT_WAIT) {
  mp3Command(0x0E, 0x00, 0x00, wait);
}

void stopMusic(unsigned long wait = DEFAULT_WAIT) {
  mp3Command(0x16, 0x00, 0x00, wait);
}


// LCD Display
// ************************************************************
#include <LiquidCrystal.h>

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
#define LCD_RS 7
#define LCD_EN 6
#define LCD_D4 2
#define LCD_D5 3
#define LCD_D6 4
#define LCD_D7 5
LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

void tagToLCD_8x2(byte tagSize) {
  lcd.clear();
  for (int i = 0; i < 4; i++) {
    lcd.setCursor(2 * i, 0);
    lcd.print(tagUID[i] >> 4, HEX);
    lcd.setCursor(2 * i + 1, 0);
    lcd.print(tagUID[i] & B00001111, HEX);
  }

  for (int i = 4; i < tagSize; i++) {
    lcd.setCursor(2 * (i - 4), 1);
    lcd.print(tagUID[i] >> 4, HEX);
    lcd.setCursor(2 * (i - 4) + 1, 1);
    lcd.print(tagUID[i] & B00001111, HEX);
  }
}

void textToLCD_8x2(char *line1, char *line2) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(line1);
  lcd.setCursor(0, 1);
  lcd.print(line2);
}

// Potentiometer
// ************************************************************
#define POT_PIN A0
int potPosition;
int potLastPosition;

uint16_t getOutputLevel( uint16_t inputLevel ) {
  const uint16_t margin = 10 ;   //  +/- 10
  const uint16_t numberOfLevelsOutput = 30 ;
  const uint16_t endPointInput[ numberOfLevelsOutput + 1 ] = { 0, 34, 68, 102, 136, 170, 204, 238, 272, 306, 340, 374, 408, 442, 476, 510, 544, 578, 612, 646, 680, 714, 748, 782, 816, 850, 884, 918, 952, 986, 1023} ;
  const  uint16_t initialOutputLevel = 0 ;
  static uint16_t currentOutputLevel = initialOutputLevel ;
  uint16_t lb = endPointInput[ currentOutputLevel ] ;
  if ( currentOutputLevel > 0 ) lb -= margin  ;   // subtract margin

  uint16_t ub = endPointInput[ currentOutputLevel + 1 ] ;
  if ( currentOutputLevel < numberOfLevelsOutput ) ub +=  margin  ;  // add margin
  if ( inputLevel < lb || inputLevel > ub ) {
    uint16_t i;
    for ( i = 0 ; i < numberOfLevelsOutput ; i++ ) {
      if ( inputLevel >= endPointInput[ i ] && inputLevel <= endPointInput[ i + 1 ] ) break ;
    }
    currentOutputLevel = i ;
  }
  return currentOutputLevel ;
}


// EEPROM
// ************************************************************
#include <EEPROM.h>
#define TAG_LENGHT 7
#define MAX_TAG_NUMBERS 100 // max 128
byte tagNumbers = 0;
byte tagUIDs[MAX_TAG_NUMBERS][TAG_LENGHT];

void tagUIDsInit(byte n) {
  for (int i = 0; i < min(n, MAX_TAG_NUMBERS); i++) {
    for (int j = 0; j < TAG_LENGHT; j++) {
      tagUIDs[i][j] = EEPROM.read(i * TAG_LENGHT + j + 1);
    }
  }
}


void setup() {
  // LCD Display
  // ************************************************************
  // set up the LCD's number of columns and rows:
  lcd.begin(8, 2);
  // Print a message to the LCD.
  textToLCD_8x2("Starting", "...");

  // RFID Reader
  // ************************************************************
  SPI.begin();          // Init SPI bus
  mfrc522.PCD_Init();   // Init MFRC522
  resetCurrentTag();

  // MP3 Player
  // ************************************************************
  //Serial.begin(115200);
  Serial.begin(9600);
  delay(1000); // delay for MP3 Player to start
  mp3Reset(500);
  mp3SelectSDCard(500);
  potPosition = getOutputLevel(analogRead(POT_PIN)) + 1;
  mp3SetVolume(potPosition, 500);

  // Potentiometer
  // ************************************************************
  potLastPosition = getOutputLevel(analogRead(POT_PIN)) + 1;

  // EEPROM
  // ************************************************************
  tagNumbers = EEPROM.read(0x00); // The first byte of the EEPROM is used to store the number of known tags
  tagUIDsInit(tagNumbers);
  textToLCD_8x2("Stored t", "ags: ");
  lcd.print(tagNumbers);
  delay(3000);

  // Let's go!
  textToLCD_8x2("Wait for", " tag ...");
}

void loop() {
  if (mfrc522.PICC_IsNewCardPresent()) {
    if (mfrc522.PICC_ReadCardSerial()) { // Select one of the cards
      is_not_removed = 2; // We have to read it twice before being sure the card is removed.

      // New tag ?
      bool is_new = false;
      for (int i = 0; i < mfrc522.uid.size; i++) {
        if (mfrc522.uid.uidByte[i] != tagUID[i]) {
          is_new = true;
        }
      }

      if (is_new) {
        // New tag!
        for (int i = 0; i < mfrc522.uid.size; i++) {
          tagUID[i] = mfrc522.uid.uidByte[i];
        }

        // is it a known tag ?
        bool is_known = false;
        byte tagNumber = 1;
        for (int i = 0; i < tagNumbers; i++) {
          is_known = true;
          for (int j = 0; j < TAG_LENGHT; j++) {
            if (tagUIDs[i][j] != tagUID[j]) {
              is_known = false;
              break;
            }
          }
          if (is_known) {
            tagNumber = i + 1;
            break;
          }
          is_known = false;
        }
        if (!is_known and (tagNumbers < MAX_TAG_NUMBERS)) {
          // A very new tag!
          tagNumbers++;
          EEPROM.write(0x00, tagNumbers);
          tagNumber = tagNumbers;
          for (int i = 0; i < TAG_LENGHT; i++) {
            tagUIDs[tagNumber - 1][i] = tagUID[i];
            EEPROM.write((tagNumber - 1) * TAG_LENGHT + i + 1, tagUID[i]);
          }
        }

        // Display it on LCD
        tagToLCD_8x2(mfrc522.uid.size);
        lcd.setCursor(6, 1);
        lcd.print(tagNumber);

        // Play a song
        if (tagNumber == lastTagNumber) {
          // Resume playing
          playResumeMusic();
        }
        else {
          // Play a new track
          byte folder = 1;
          mp3PlaySong(folder, tagNumber);
          lastTagNumber = tagNumber;
        }
      }
    }
  }
  else {
    // One on two reads the card is not detected.
    // We have to read it twice before being sure the card is removed.
    if (is_not_removed) {
      if (is_not_removed == 1) {
        // Card is removed!

        // Stop the music!
        //stopMusic();
        pauseMusic();

        resetCurrentTag();
        textToLCD_8x2("Wait for", " tag ...");
      }
      is_not_removed--;
    }
    return; // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  }

  // Change volume ?
  potPosition = getOutputLevel(analogRead(POT_PIN)) + 1;
  if (potPosition != potLastPosition) {
    mp3SetVolume(potPosition);
    potLastPosition = potPosition;
  }

  delay(10); // for stability ...
}
