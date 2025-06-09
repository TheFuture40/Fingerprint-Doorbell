# Fingerprint-Doorbell
The system detects the right fingerprint and opens the door.
# 🔐 Arduino Fingerprint-Based Door Lock System

A secure fingerprint-based door locking system built using an **Arduino**, **fingerprint sensor**, **LCD display**, and **solenoid lock**. When an authorized fingerprint is scanned, the solenoid lock unlocks for 5 seconds before re-locking. All status messages are shown clearly on a 16x2 LCD.

---

## 🧰 Components Used

| Component              | Purpose                        |
|------------------------|--------------------------------|
| Arduino Uno  | Main microcontroller           |
| Fingerprint Sensor (R305) | Biometric authentication     |
| Solenoid Lock          | Door locking mechanism         |
| Relay Module or NPN Transistor | Drives solenoid from Arduino |
| 16x2 I2C LCD           | Displays system status         |
| External Power Supply  | For solenoid (12V)     |

---

## 🔧 Pin Configuration

| Arduino Pin | Connected To           | Description               |
|-------------|------------------------|---------------------------|
| 2           | Fingerprint TX         | Serial RX for fingerprint |
| 3           | Fingerprint RX         | Serial TX for fingerprint |
| 8           | Relay or Transistor IN | Controls solenoid lock    |
| SDA/SCL     | I2C LCD                | Displays messages         |

---

## 📟 LCD Messages

| LCD Output              | Meaning                              |
|--------------------------|--------------------------------------|
| `Security System`<br>`PLACE FINGER` | Waiting for user input         |
| `Access Granted`         | Finger recognized, unlocking door   |
| `Door Locked`            | Lock has re-engaged                 |
| `Access Denied`          | Unknown fingerprint                 |
| `Sensor Failed`          | Fingerprint sensor not responding   |

---

## 📋 Features

- 🔒 Unlocks only for **recognized fingerprints**
- 🔁 Auto re-locks after **5 seconds**
- 🖥 Real-time feedback on **LCD display**
- 🚨 Displays failure if sensor isn't connected or fails

---

## 🧠 How It Works

1. The system initializes the fingerprint sensor, LCD, and solenoid lock.
2. The LCD continuously displays:
3. When a recognized fingerprint is scanned:
- It prints “Access Granted”
- Activates the lock (via a relay/transistor)
- Waits 5 seconds, then relocks and shows “Door Locked”
4. If the fingerprint is not recognized:
- Shows “Access Denied”
- Waits briefly, then returns to idle state

---

## 🛠 Code Overview

```cpp
#include <Adafruit_Fingerprint.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

#define SOLENOID_PIN 8

SoftwareSerial fingerSerial(2, 3); 
Adafruit_Fingerprint finger(&fingerSerial);
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
Serial.begin(9600);
lcd.begin();
lcd.backlight();
pinMode(SOLENOID_PIN, OUTPUT);
digitalWrite(SOLENOID_PIN, LOW);

finger.begin(57600);
lcd.setCursor(0, 0);
lcd.print("Security System");

if (finger.verifyPassword()) {
 lcd.setCursor(0, 1);
 lcd.print("PLACE FINGER");
} else {
 lcd.setCursor(0, 1);
 lcd.print("Sensor Failed");
 while (1);
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
 digitalWrite(SOLENOID_PIN, HIGH);
 delay(5000);
 digitalWrite(SOLENOID_PIN, LOW);
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
