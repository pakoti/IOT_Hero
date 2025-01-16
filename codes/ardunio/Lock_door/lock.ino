#include <Keypad.h>
#include <LiquidCrystal.h>
#include <Servo.h>
 
// Define pins for various components
#define SERVO_PIN 8           // Servo motor pin
#define RED_LED_PIN 12        // Red LED pin (for incorrect password indication)
#define GREEN_LED_PIN 13      // Green LED pin (for access granted indication)
#define BUZZER_PIN 11         // Buzzer pin (for sound feedback)
 
Servo myservo;                // Servo object
LiquidCrystal lcd(2, 3, 4, 5, 6, 7); // LCD object with defined pins
 
// Keypad configuration
const byte rows = 4, cols = 4; // Keypad dimensions (4x4)
char keys[rows][cols] = {      // Keypad layout
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[rows] = {A0, A1, A2, A3}; // Row pins connected to Arduino
byte colPins[cols] = {A4, A5, 9, 10};  // Column pins connected to Arduino
 
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, rows, cols); // Keypad object
 
// Passwords
char masterPassword[] = "1234";  // Master password for password change
char savedPassword[5] = "";      // User-set password
char enteredPassword[5];         // Buffer for entering a password
int currentPosition = 0;         // Tracks the number of entered digits
int invalidCount = 0;            // Counts incorrect password attempts
 
// Flags for different modes
bool isFirstBoot = true;         // Indicates if it's the first time setting the password
bool isSettingPassword = false; // Indicates if the system is in password setup mode
bool isCheckingMaster = false;  // Indicates if the system is verifying the master password
 
void setup() {
  lcd.begin(16, 2);            // Initialize LCD
  pinMode(RED_LED_PIN, OUTPUT); // Set LED pins as outputs
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);  // Set buzzer pin as output
  myservo.attach(SERVO_PIN);    // Attach servo to the defined pin
  myservo.write(0);             // Initial servo position (locked)
  Serial.begin(9600);           // Begin serial communication (for debugging)
 
  // First boot behavior: prompt user to set the password
  if (isFirstBoot) {
    lcd.print("Set Password:"); // Show setup message on LCD
    isSettingPassword = true;  // Enable password setup mode
    currentPosition = 0;       // Reset the current position
  } else {
    lcd.print("*ENTER THE CODE*"); // Prompt user to enter the code
  }
}
 
void loop() {
  char key = keypad.getKey(); // Read key pressed on the keypad
  if (key) {
    playKeyPressSound();      // Play sound feedback for any key press
 
    if (isSettingPassword) {
      handlePasswordSetup(key); // Handle password setup mode
    } else if (isCheckingMaster) {
      handleMasterPasswordCheck(key); // Handle master password verification
    } else {
      handlePasswordEntry(key); // Handle normal password entry
    }
  }
}
 
// Function to play sound feedback for a key press
void playKeyPressSound() {
  tone(BUZZER_PIN, 1000, 100); // Play 1kHz tone for 100ms
  delay(100);                  // Ensure sound is distinct
}
 
// Function to handle password setup
void handlePasswordSetup(char key) {
  lcd.clear();                 // Clear LCD for updated display
 
  if (key == 'B') {            // Delete the last character
    if (currentPosition > 0) {
      currentPosition--;       // Move back one position
      enteredPassword[currentPosition] = '\0'; // Clear the character
    }
  } else if (key >= '0' && key <= '9' && currentPosition < 4) {
    // Add digit to entered password if less than 4 digits
    enteredPassword[currentPosition] = key;
    currentPosition++;
  } else if (key == 'C') {     // Confirm the entered password
    if (currentPosition == 4) { // Check if 4 digits are entered
      enteredPassword[4] = '\0'; // Null-terminate the string
      strncpy(savedPassword, enteredPassword, 5); // Save new password
      lcd.print("Password Set!"); // Display success message
      delay(2000);
      isSettingPassword = false; // Exit password setup mode
      isFirstBoot = false;       // First boot completed
      lcd.clear();
      lcd.print("*ENTER THE CODE*"); // Go back to normal mode
      currentPosition = 0;       // Reset position for new entries
    } else {
      lcd.print("Incomplete Pass"); // Warn user of incomplete entry
      delay(2000);
    }
    return;
  }
 
  // Display entered digits during password setup
  lcd.setCursor(0, 0);
  lcd.print("Set New Pass:");
  lcd.setCursor(0, 1);
  for (int i = 0; i < currentPosition; i++) {
    lcd.print(enteredPassword[i]); // Show the actual digits
  }
}
 
// Function to handle master password verification for password change
void handleMasterPasswordCheck(char key) {
  lcd.clear();                 // Clear LCD for updated display
 
  if (key == 'B') {            // Delete the last character
    if (currentPosition > 0) {
      currentPosition--;       // Move back one position
      enteredPassword[currentPosition] = '\0'; // Clear the character
    }
  } else if (key >= '0' && key <= '9' && currentPosition < 4) {
    // Add digit to entered password if less than 4 digits
    enteredPassword[currentPosition] = key;
    currentPosition++;
  } else if (key == 'C') {     // Confirm the entered password
    if (currentPosition == 4) { // Check if 4 digits are entered
      enteredPassword[4] = '\0'; // Null-terminate the string
      if (strcmp(enteredPassword, masterPassword) == 0) {
        lcd.print("Master Verified"); // Display success message
        delay(2000);
        isCheckingMaster = false; // Exit master password check
        isSettingPassword = true; // Enter password setup mode
        currentPosition = 0;      // Reset position for new entries
        lcd.clear();
        lcd.print("Set Password:"); // Prompt to set new password
      } else {
        lcd.print("Incorrect Master"); // Display error message
        delay(2000);
        isCheckingMaster = false; // Exit master password check
        currentPosition = 0;      // Reset position
        lcd.clear();
        lcd.print("*ENTER THE CODE*"); // Go back to normal mode
      }
    } else {
      lcd.print("Incomplete Pass"); // Warn user of incomplete entry
      delay(2000);
    }
    return;
  }
 
  // Display asterisks during master password entry
  lcd.setCursor(0, 0);
  lcd.print("Master Pass:");
  lcd.setCursor(0, 1);
  for (int i = 0; i < currentPosition; i++) {
    lcd.print("*");            // Show asterisks for entered digits
  }
}
 
// Function to handle normal password entry
void handlePasswordEntry(char key) {
  if (key == 'A') {            // Initiate password change
    lcd.clear();
    lcd.print("Master Pass:");
    currentPosition = 0;       // Reset position
    isCheckingMaster = true;   // Enter master password check mode
    return;
  } else if (key == 'B') {     // Delete the last character
    if (currentPosition > 0) {
      currentPosition--;       // Move back one position
      enteredPassword[currentPosition] = '\0'; // Clear the character
    }
  } else if (key >= '0' && key <= '9' && currentPosition < 4) {
    // Add digit to entered password if less than 4 digits
    enteredPassword[currentPosition] = key;
    currentPosition++;
  } else if (key == 'C') {     // Confirm the entered password
    if (currentPosition == 4) { // Check if 4 digits are entered
      enteredPassword[4] = '\0'; // Null-terminate the string
      if (strcmp(enteredPassword, savedPassword) == 0) {
        unlockDoor();          // Unlock the door if password is correct
      } else {
        incorrectPassword();   // Indicate incorrect password
      }
      currentPosition = 0;     // Reset position for new entries
    } else {
      lcd.clear();
      lcd.print("Incomplete Pass"); // Warn user of incomplete entry
      delay(2000);
      lcd.clear();
      lcd.print("*ENTER THE CODE*"); // Go back to normal mode
    }
    return;
  }
 
  // Display asterisks during normal password entry
  lcd.clear();
  lcd.print("PASSWORD:");
  lcd.setCursor(0, 1);
  for (int i = 0; i < currentPosition; i++) {
    lcd.print("*");            // Show asterisks for entered digits
  }
}
 
// Function to unlock the door
void unlockDoor() {
  lcd.clear();
  lcd.print("Access Granted");
  digitalWrite(GREEN_LED_PIN, HIGH); // Turn on the green LED
 
  // Play success melody
  for (int i = 0; i < 4; i++) {
    tone(BUZZER_PIN, 400 + (i * 200), 200);
    delay(250);
  }
  noTone(BUZZER_PIN);
  digitalWrite(GREEN_LED_PIN, LOW); // Turn off the green LED
 
  delay(2500); // Display "Access Granted" for 2.5 seconds
 
  // Unlock the door and show countdown timer
  myservo.write(90); // Unlock position
  for (int secondsLeft = 10; secondsLeft > 0; secondsLeft--) {
    lcd.clear();
    lcd.print("Door locks in:");
    lcd.setCursor(6, 1);
    lcd.print(secondsLeft);   // Show countdown timer
    lcd.print(" sec");
    delay(1000);
  }
 
  // Lock the door
  lcd.clear();
  lcd.print("Locking Door...");
  myservo.write(0);          // Lock position
  delay(1000);
  lcd.clear();
  lcd.print("*ENTER THE CODE*"); // Go back to normal mode
}
 
// Function to handle incorrect password entry
void incorrectPassword() {
  lcd.clear();
  lcd.print("CODE INCORRECT");
  digitalWrite(RED_LED_PIN, HIGH); // Turn on the red LED
 
  // Play alert melody
  for (int i = 0; i < 3; i++) {
    tone(BUZZER_PIN, 300, 150);
    delay(200);
    tone(BUZZER_PIN, 100, 150);
    delay(200);
  }
  noTone(BUZZER_PIN);
  digitalWrite(RED_LED_PIN, LOW); // Turn off the red LED
 
  delay(2500); // Display "CODE INCORRECT" for 2.5 seconds
 
  lcd.clear();
  lcd.print("*ENTER THE CODE*"); // Go back to normal mode
}