#include "SPI.h"
#include "LiquidCrystal.h"
//remove the spaces above
 
// Initialize the LCD screen using the latch pin
LiquidCrystal lcd(10);
 
void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
//  lcd.print("hello world!");

}
 
void loop() {
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(1, 0);
  // print the number of seconds since reset:
  lcd.print("hehehe");
}
