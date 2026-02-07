#include "Config.h"
#include "Peripherals.h"
#include "Comms.h"
#include "FSM.h"

void setup() {
  Serial.begin(115200);
  delay(100);

  initPeripherals();
  initComms();
  changeState(STATE_IDLE);
}

void loop() {
  runSystemFSM();

  char key = keypad.getKey();
  if (key) {
    handleKeypadInput(key);
  }
}
