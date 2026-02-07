#ifndef POWER_H
#define POWER_H

#include "Config.h"
#include "Peripherals.h"

// unsigned long enterLightSleep() {
//   Serial.println("Entering Light Sleep (LPM0 mode)...");
//   
//   delay(20); 
//   lcd.noBacklight(); 
//
//   setKeypadIdeallyLow();
//
//   esp_sleep_enable_ext0_wakeup(INTERRUPT_PIN, 0); 
//
//   esp_light_sleep_start();
//
//   Serial.println("Resumed!");
//
//   lcd.backlight();
//   lcd.setCursor(0, 1);
//   lcd.print("Woke up!       ");
//   
//   // Return current time to reset the activity timer
//   return millis();
// }
// ... inside Power.h ...

unsigned long enterLightSleep() {
  Serial.println("Entering Light Sleep...");

  delay(20);
  lcd.noBacklight();

  esp_sleep_enable_gpio_wakeup();

  gpio_wakeup_enable((gpio_num_t)INTERRUPT_PIN, GPIO_INTR_LOW_LEVEL);
  gpio_wakeup_enable((gpio_num_t)PIN_DOOR_SIM, GPIO_INTR_LOW_LEVEL);
  gpio_wakeup_enable((gpio_num_t)PIN_MOTION_SIM, GPIO_INTR_LOW_LEVEL);

  setKeypadIdeallyLow();

  esp_light_sleep_start();

  // ---------------- WAKE UP HAPPENS HERE ----------------

  Serial.println("Resumed!");

  lcd.backlight();
  lcd.setCursor(0, 1);
  lcd.print("System Awake    ");

  return millis();
}
#endif
