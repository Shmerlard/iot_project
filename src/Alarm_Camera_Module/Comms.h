#ifndef COMMS_H
#define COMMS_H

#include <Arduino.h>


#define COMM_RX_PIN 2
#define COMM_TX_PIN 1
#define COMM_BAUD   9600
#define BUTTON_PIN  14

const char CMD_CHECK_FACE = 'C'; 
const char RESP_AUTHORIZED = 'K'; 
const char RESP_INTRUDER   = 'A'; 

void initComms() {
  Serial2.begin(COMM_BAUD, SERIAL_8N1, COMM_RX_PIN, COMM_TX_PIN);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  Serial.println("[Comms] Camera Listener Ready on Pins 2(RX)/1(TX)");
  Serial.println("[Comms] Manual Trigger Ready on Pin 14");
}

bool checkForTrigger() {
  if (Serial2.available()) {
    char cmd = Serial2.read();
    Serial.print("[Comms] RAW DATA RECEIVED: "); // Debug print
    Serial.println(cmd);

    if (cmd == CMD_CHECK_FACE) return true;
  }
  if (digitalRead(BUTTON_PIN) == LOW) {
      Serial.println("[Event] Manual Button Triggered!");

      while(digitalRead(BUTTON_PIN) == LOW) { delay(50); }

      return true;
  }
  return false;
}

void sendVerdict(bool isAuthorized) {
  if (isAuthorized) {
    Serial.println("[Comms] Verdict: AUTHORIZED. Sending 'K'.");
    Serial2.write(RESP_AUTHORIZED);
  } else {
    Serial.println("[Comms] Verdict: INTRUDER. Sending 'A'.");
    Serial2.write(RESP_INTRUDER);
  }
}

#endif
