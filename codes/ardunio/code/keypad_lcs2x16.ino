#include <LiquidCrystal.h>
#include <Keypad.h>

// LCD setup
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Keypad setup
const byte ROWS = 4; // Four rows
const byte COLS = 4; // Four columns
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = {A0, A1, A2, A3}; // Connect to keypad R1-R4
byte colPins[COLS] = {9, 8, 7, 6};     // Connect to keypad C1-C4

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup() {
  // Initialize LCD
  lcd.begin(16, 2);
  lcd.print("Keypad Test");
  lcd.setCursor(0, 1);
  lcd.print("Press a key...");
}

void loop() {
  char key = keypad.getKey();
  
  if (key) {
    lcd.clear();
    lcd.print("You pressed:");
    lcd.setCursor(0, 1);
    lcd.print(key);
    

  }
}