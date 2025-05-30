#include <LiquidCrystal.h>
LiquidCrystal lcd(2,3,4,5,6,7); // pins for RS, E, DB4, DB5, DB6, DB7

void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);

}

void loop() {
  //   
  lcd.print("hello");
  lcd.setCursor(0, 1); // ستون 0، ردیف دوم
  lcd.print("wprld!");
  delay(50000);
}
