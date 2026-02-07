#ifndef FSM_H
#define FSM_H

#include "Config.h"
#include "Peripherals.h"
#include "Comms.h"
#include "Power.h"

unsigned long lastActivityTime = 0;
// --- 1. Define The States ---
enum SystemState {
  STATE_IDLE,
  STATE_EXIT_DELAY, 
  STATE_ARMED,
  STATE_ENTRY_DELAY,
  STATE_ALARM
};

SystemState currentState = STATE_IDLE;
unsigned long stateStartTime = 0; 
String inputBuffer = "";

// --- 2. State Changer ---

void changeState(SystemState newState) {
  lastActivityTime = millis();
  currentState = newState;
  stateStartTime = millis();
  inputBuffer = "";
  flushSerialBuffer();
  lcd.clear();

  switch (currentState) {
    case STATE_IDLE:
      lcd.print("System Disarmed");
      triggerAlarm(false);
      lcd.setCursor(0, 1);
      lcd.print("Ready to Arm");
      break;

    case STATE_EXIT_DELAY:
      lcd.print("Arming System...");
      lcd.setCursor(0, 1);
      lcd.print("Please Exit");
      break;

    case STATE_ARMED:
      lcd.print("SYSTEM ARMED");
      lcd.setCursor(0, 1);
      lcd.print("Monitoring...");
      break;

    case STATE_ENTRY_DELAY:
      lcd.print("DOOR OPENED!");
      lcd.setCursor(0, 1);
      lcd.print("Face ID / Pass:");

      requestFaceCheck(); 
      break;

    case STATE_ALARM:
      lcd.print("!!! ALARM !!!");
      lcd.setCursor(0, 1);
      triggerAlarm(true);
      requestFaceCheck();
      lcd.print("Police Notified");
      break;
  }
}

void handleKeypadInput(char key) {
  lastActivityTime = millis(); // <--- ADD THIS (Reset timer on key press)
  if (key == '#') {
    if (inputBuffer == MASTER_PASSWORD) {
      if (currentState == STATE_IDLE) {
        changeState(STATE_EXIT_DELAY); 
      } 
      else if (currentState == STATE_ENTRY_DELAY || currentState == STATE_ALARM || currentState == STATE_EXIT_DELAY) {
        changeState(STATE_IDLE); 
      }
    } else {
      clearRow(1);
      lcd.print("Wrong Password!");
      delay(1000);
      inputBuffer = "";
      changeState(currentState);
    }
  } 
  else if (key == '*') {
    inputBuffer = "";
    clearRow(2);
  } 
  else {
    inputBuffer += key;
    lcd.setCursor(inputBuffer.length() - 1, 2); 
    lcd.print('*');
  }
}

void runSystemFSM() {
  unsigned long timeInState = millis() - stateStartTime;

  // Check for Camera updates in any relevant state
  if (currentState == STATE_ENTRY_DELAY || currentState == STATE_ALARM) {
      char verdict = checkIncomingMessages();

      if (verdict == 'K') { // Camera says: "Authorized"
          showMessage("Face Recognized!", "Welcome Home.");
          triggerAlarm(false); // Silence the alarm immediately!
          delay(2000);
          changeState(STATE_IDLE); // Auto-Disarm
      }
      else if (verdict == 'A') { // Camera says: "Intruder"
          if (currentState != STATE_ALARM && currentState != STATE_ALARM)
            changeState(STATE_ALARM); // Instant Alarm
      }
  }
  if (currentState == STATE_IDLE || currentState == STATE_ARMED) {
      
      // If 20 seconds have passed since last button press
      if (millis() - lastActivityTime > 20000) {
          if (!checkDoorOpen() && !checkMotion()) {
              lastActivityTime = enterLightSleep();

              stateStartTime = millis(); 
          }
      }
  }
  // --- State Logic ---
  switch (currentState) {
    case STATE_IDLE:
      break;

    case STATE_EXIT_DELAY:
      lcd.setCursor(0, 3);
      lcd.print("Arming in: " + String((EXIT_DELAY_MS - timeInState) / 1000) + "s ");
      if (timeInState > EXIT_DELAY_MS) {
        changeState(STATE_ARMED);
      }
      break;

    case STATE_ARMED:
      if (checkMotion()) {
        lcd.clear();
        lcd.print("MOTION DETECTED!");
        delay(500);
        changeState(STATE_ALARM);
      }
      else if (checkDoorOpen()) {
        changeState(STATE_ENTRY_DELAY);
      }
      break;

    case STATE_ENTRY_DELAY:
      lcd.setCursor(14, 1);
      lcd.print((ALARM_DELAY_MS - timeInState) / 1000);
      lcd.print("s ");

      if (timeInState > ALARM_DELAY_MS) {
        changeState(STATE_ALARM);
      }
      break;

    case STATE_ALARM:
      break;
  }
}

#endif
