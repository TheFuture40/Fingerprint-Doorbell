#include <Adafruit_Fingerprint.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

// Pin setup
#define SOLENOID_PIN 8

// Fingerprint sensor RX/TX to Arduino pins 2 and 3
SoftwareSerial fingerSerial(2, 3); 
Adafruit_Fingerprint finger(&fingerSerial);

// LCD: address 0x27, 16 chars, 2 lines
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  Serial.begin(9600);              // For debug output
  lcd.begin();                     // Initialize LCD
  lcd.backlight();                 // Turn on LCD backlight

  pinMode(SOLENOID_PIN, OUTPUT);
  digitalWrite(SOLENOID_PIN, LOW); // Lock is off by default

  // Initialize fingerprint sensor
  finger.begin(57600);
  lcd.setCursor(0, 0);
  lcd.print("Security System");

  if (finger.verifyPassword()) {
    lcd.setCursor(0, 1);
    lcd.print("PLACE FINGER");
  } else {
    lcd.setCursor(0, 1);
    lcd.print("Sensor Failed");
    while (1); // Stop if sensor not responding
  }

  delay(2000);
  lcd.clear();
}

void loop() {
  lcd.setCursor(0, 0);
  lcd.print("Security System");
  lcd.setCursor(0, 1);
  lcd.print("PLACE FINGER   ");

  int result = getFingerprintID();

  if (result == FINGERPRINT_OK) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Access Granted");
    digitalWrite(SOLENOID_PIN, HIGH);  // Unlock door
    delay(5000);                        // Keep unlocked
    digitalWrite(SOLENOID_PIN, LOW);   // Lock again

    lcd.setCursor(0, 1);
    lcd.print("Door Locked");
    delay(2000);
  } else if (result == FINGERPRINT_NOTFOUND) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Access Denied");
    delay(2000);
  }

  lcd.clear();
}

int getFingerprintID() {
  int p = finger.getImage();
  if (p != FINGERPRINT_OK) return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) return -1;

  p = finger.fingerSearch();
  if (p != FINGERPRINT_OK) return FINGERPRINT_NOTFOUND;

  Serial.print("Found ID #");
  Serial.println(finger.fingerID);
  return FINGERPRINT_OK;
}