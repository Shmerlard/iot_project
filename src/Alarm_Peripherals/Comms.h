#ifndef COMMS_H
#define COMMS_H

#include <Arduino.h>
#include "Peripherals.h" 

#define RX_PIN 18
#define TX_PIN 5

void initComms() {
  // Restarting Serial2 with new pins
  Serial2.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);
  Serial.println("[Comms] Alarm Serial initialized on 18(RX)/5(TX)");
  delay(50);
  // Serial2.println("BOOT_READY");
}

void requestFaceCheck() {
  Serial.println("[Comms] Sending Check Request ('C')...");
  Serial2.write('C'); 
}
void flushSerialBuffer() {
  while (Serial2.available() > 0) {
    char trash = Serial2.read(); 
  }
}
char checkIncomingMessages() {
  if (Serial2.available()) {
    char message = Serial2.read();
    Serial.print("[Comms] Received: ");
    Serial.println(message);
    return message;
  }
  return 0;
}

#endif
