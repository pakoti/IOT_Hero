#include <Wire.h>
#include <RTClib.h>
#include <LiquidCrystal.h>

// Initialize RTC and LCD objects
RTC_DS3231 rtc;
LiquidCrystal lcd(12, 11, 5, 4, 3, 2); // RS, E, D4, D5, D6, D7

void setup() {
  Serial.begin(9600);
  
  // Initialize LCD
  lcd.begin(16, 2);
  lcd.clear();
  lcd.print("Initializing...");
  
  // Initialize RTC
  if (!rtc.begin()) {
    lcd.clear();
    lcd.print("RTC Not Found!");
    while (1);
  }
  
  if (rtc.lostPower()) {
    lcd.setCursor(0, 1);
    lcd.print("Setting Time...");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    delay(1000);
  }
  
  lcd.clear();
}

void loop() {
  DateTime now = rtc.now();
  
  // Display Date on first line
  lcd.setCursor(0, 0);
  printTwoDigits(now.day());
  lcd.print("/");
  printTwoDigits(now.month());
  lcd.print("/");
  lcd.print(now.year());
  
  // Display Time on second line
  lcd.setCursor(0, 1);
  printTwoDigits(now.hour());
  lcd.print(":");
  printTwoDigits(now.minute());
  lcd.print(":");
  printTwoDigits(now.second());
  
  delay(1000);
}

void printTwoDigits(int number) {
  if (number < 10) {
    lcd.print("0");
  }
  lcd.print(number);
}
