#include <ServoTimer2.h>  // ✅ Use ServoTimer2 instead of Servo.h
#include <SoftwareSerial.h>
#include <avr/wdt.h> 

#define WEAPON_PIN 3      // Weapon motor ESC pin
#define RIGHT_RPWM 6      // Right wheel forward
#define RIGHT_LPWM 5      // Right wheel backward
#define LEFT_RPWM 9       // Left wheel forward
#define LEFT_LPWM 10      // Left wheel backward

SoftwareSerial BTSerial(12, 11);  // RX, TX (HC-05)
ServoTimer2 weapon;  

bool weaponInitialized = false;   // ✅ Flag to track if weapon is armed

void setup() {
  Serial.begin(9600);
  BTSerial.begin(9600);

  // Weapon motor setup
  weapon.attach(WEAPON_PIN);

  // Wheel motor pins
  pinMode(RIGHT_RPWM, OUTPUT);
  pinMode(RIGHT_LPWM, OUTPUT);
  pinMode(LEFT_RPWM, OUTPUT);
  pinMode(LEFT_LPWM, OUTPUT);

  stopMotors();

  Serial.println("✅ Battlebot Ready! Waiting for Bluetooth commands...");
}

void loop() {
  if (BTSerial.available()) {
    char receivedChar = BTSerial.read();

    switch (receivedChar) {
      // ✅ Movement Controls (Only moves while pressed)
      case 'F': moveForward(); break;  
      case 'B': moveBackward(); break;
      case 'L': turnLeft(); break;
      case 'R': turnRight(); break;
      case '0': stopMotors(); break;   // Stops moving when button is released

      // ✅ Weapon Controls
      case 'S':  
        if (!weaponInitialized) {
          initializeWeapon();
          weaponInitialized = true;
          Serial.println("🔧 Weapon Armed!");
        } else {
          Serial.println("⚠️ Weapon already initialized.");
        }
        break;

      case 'T':  
        if (weaponInitialized) {
          spinWeaponSlow();
          Serial.println("🔥 Spinning Weapon (95°)");
        } else {
          Serial.println("⚠️ Weapon not initialized yet.");
        }
        break;

      case 'C':  
        stopWeapon();
        Serial.println("🛑 Weapon Stopped");
        break;

      case 'X':  
        maxWeapon();
        Serial.println("Max weapon speed");
        break;

      case 'A':   // ✅ Start button triggers reset
        Serial.println("🔄 Resetting Arduino...");
        delay(100);      // Small delay to print the message
        wdt_enable(WDTO_15MS);  // Enable watchdog timer with 15ms timeout
        while (1) {}     // Wait for the watchdog to reset
        break;

      default:
        Serial.print("Unknown command: ");
        Serial.println(receivedChar, HEX);
        stopMotors();
        break;
    }
  }
}

// ✅ **Weapon Initialization**
void initializeWeapon() {
  Serial.println("🔌 Arming Weapon ESC...");

  // 1. **Neutral Throttle (93°) → ServoTimer2 equivalent**
  weapon.write(1510);   // 93° equivalent with ServoTimer2
  Serial.println("⚙️ Neutral throttle (93°) → 3 sec");
  delay(3000);

  // ✅ Return to Neutral
  Serial.println("✅ Weapon Armed at Neutral");
  weapon.write(1510);   // Neutral throttle
}

// ✅ **Spin Weapon Slowly**
void spinWeaponSlow() {
  weapon.write(1440);   // 90° equivalent with ServoTimer2 (low throttle spin)
}

// ✅ **Stop Weapon**
void stopWeapon() {
  weapon.write(1510);   // Set to neutral throttle
  delay(500);           // Let ESC recognize neutral
  weapon.write(1510);   // Double confirmation
}


// ✅ **Max Weapon Spin**
void maxWeapon() {
  weapon.write(1440);   // 95° equivalent with ServoTimer2 (max throttle)
}

// ✅ **Movement Functions**
void moveForward() {
  analogWrite(RIGHT_RPWM, 200);
  analogWrite(RIGHT_LPWM, 0);
  analogWrite(LEFT_RPWM, 200);
  analogWrite(LEFT_LPWM, 0);
}

void moveBackward() {
  analogWrite(RIGHT_RPWM, 0);
  analogWrite(RIGHT_LPWM, 200);
  analogWrite(LEFT_RPWM, 0);
  analogWrite(LEFT_LPWM, 200);
}

void turnLeft() {
  analogWrite(RIGHT_RPWM, 200);
  analogWrite(RIGHT_LPWM, 0);
  analogWrite(LEFT_RPWM, 0);
  analogWrite(LEFT_LPWM, 200);
}

void turnRight() {
  analogWrite(RIGHT_RPWM, 0);
  analogWrite(RIGHT_LPWM, 200);
  analogWrite(LEFT_RPWM, 200);
  analogWrite(LEFT_LPWM, 0);
}

void stopMotors() {
  digitalWrite(RIGHT_RPWM, LOW);
  digitalWrite(RIGHT_LPWM, LOW);
  digitalWrite(LEFT_RPWM, LOW);
  digitalWrite(LEFT_LPWM, LOW);
}
