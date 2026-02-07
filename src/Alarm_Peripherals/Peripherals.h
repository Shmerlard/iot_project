#ifndef PERIPHERALS_H
#define PERIPHERALS_H

#include "Config.h"
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

// Global Objects
LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);
Keypad keypad = Keypad(makeKeymap(KEY_MAP), (byte*)ROW_PINS, (byte*)COL_PINS, ROWS, COLS);

void initPeripherals() {
  Wire.begin(I2C_SDA, I2C_SCL); 
  lcd.init();
  lcd.backlight();

  // Configure Buttons (Input Pullup means LOW when pressed)
  pinMode(PIN_MOTION_SIM, INPUT_PULLUP);
  pinMode(PIN_DOOR_SIM, INPUT_PULLUP);

  pinMode(ALARM_PIN, OUTPUT);
  digitalWrite(ALARM_PIN, LOW); // Start with LED OFF
  // Configure Interrupt Pin
  pinMode(INTERRUPT_PIN, INPUT_PULLUP);
}

// --- Sleep Helpers ---

void setKeypadIdeallyLow() {
  for (int i = 0; i < ROWS; i++) {
    pinMode(ROW_PINS[i], OUTPUT);
    digitalWrite(ROW_PINS[i], LOW); 
  }
}

// --- FSM Helpers ---
void triggerAlarm(bool state) {
    if (state) {
        digitalWrite(ALARM_PIN, HIGH); // Turn LED ON
    } else {
        digitalWrite(ALARM_PIN, LOW);  // Turn LED OFF
    }
}

void clearRow(int row) {
  lcd.setCursor(0, row);
  lcd.print("                    ");
  lcd.setCursor(0, row);
}

bool checkDoorOpen() { return digitalRead(PIN_DOOR_SIM) == LOW; }
bool checkMotion()   { return digitalRead(PIN_MOTION_SIM) == LOW; }

void showMessage(String line1, String line2 = "") {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(line1);
  if (line2 != "") {
    lcd.setCursor(0, 1);
    lcd.print(line2);
  }
}

#endif
