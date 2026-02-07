#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// --- I2C / LCD Settings ---
#define LCD_ADDR 0x27
#define LCD_COLS 20
#define LCD_ROWS 4
#define I2C_SDA  21
#define I2C_SCL  19

// --- Keypad Settings ---
const byte ROWS = 4;
const byte COLS = 3;

const byte ROW_PINS[ROWS] = { 12, 33, 25, 27 };
const byte COL_PINS[COLS] = { 14, 13, 26 };

const char KEY_MAP[ROWS][COLS] = {
  { '1', '2', '3' },
  { '4', '5', '6' },
  { '7', '8', '9' },
  { '*', '0', '#' }
};

// --- Security Settings ---
#define PIN_MOTION_SIM  15   // "Someone inside"
#define PIN_DOOR_SIM    4   // "Door Open"
#define ALARM_PIN       22

#define MASTER_PASSWORD "1234"
#define EXIT_DELAY_MS   10000 // 10 seconds to leave house
#define ALARM_DELAY_MS  15000 // 15 seconds to enter password before alarm

// --- Power Settings ---
#define INTERRUPT_PIN   GPIO_NUM_32
#define WAKE_THRESHOLD  5000

#endif
