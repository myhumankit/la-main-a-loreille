#include <LiquidCrystal.h>

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
#define LCD_RS 8
#define LCD_EN 9
#define LCD_D4 4
#define LCD_D5 5
#define LCD_D6 6
#define LCD_D7 7
#define LCD_Backlight 10
LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

void setup() {
  // set up the backlight to full brightness:
  pinMode(LCD_Backlight, OUTPUT);
  analogWrite(LCD_Backlight, 255);

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);

  // print a message to the LCD.
  lcd.setCursor(0, 0);
  lcd.print("Hello World !");
}

void loop() {

}
