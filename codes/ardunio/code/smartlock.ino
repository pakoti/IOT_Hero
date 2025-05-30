#include <Keypad.h>
#include <LiquidCrystal.h>

// Keypad setup
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = {A0, A1, A2, A3};
byte colPins[COLS] = {9, 8, 7, 6};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// LCD setup
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// Relay setup
const int relayPin = 10;

// Password setup
String password = "1234"; // Change this!
String inputPassword;

void setup() {
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW); // Keep relay OFF initially
  
  lcd.begin(16, 2);
  lcd.print("Enter Password:");
  lcd.setCursor(0, 1);
}

void loop() {
  char key = keypad.getKey();
  
  if (key) {
    if (key == '#') { // Submit password
      checkPassword();
    } 
    else if (key == '*') { // Clear input
      inputPassword = "";
      lcd.clear();
      lcd.print("Enter Password:");
      lcd.setCursor(0, 1);
    } 
    else { // Append to password
      inputPassword += key;
      lcd.print('*'); // Show asterisk instead of actual key
    }
  }
}

void checkPassword() {
  if (inputPassword == password) {
    lcd.clear();
    lcd.print("Access Granted!");
    digitalWrite(relayPin, HIGH); // Unlock door
    delay(3000); // Keep unlocked for 3 sec
    digitalWrite(relayPin, LOW); // Lock again
    lcd.clear();
    lcd.print("Enter Password:");
    lcd.setCursor(0, 1);
  } else {
    lcd.clear();
    lcd.print("Wrong Password!");
    delay(1000);
    lcd.clear();
    lcd.print("Enter Password:");
    lcd.setCursor(0, 1);
  }
  inputPassword = ""; // Reset input
}