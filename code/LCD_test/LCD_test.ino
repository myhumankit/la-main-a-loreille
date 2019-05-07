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

void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(8, 2);
  // Print a message to the LCD.
  lcd.setCursor(0, 0);
  lcd.print("Hello");
  lcd.setCursor(0, 1);
  lcd.print("World !");
}

void loop() {

}
