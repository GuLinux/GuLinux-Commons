#pragma once

#include <Arduino.h>

class BlinkLed {
public:
    BlinkLed(int pin=LED_BUILTIN, int initial_state=HIGH) : pin(pin), initial_state(initial_state) {
      digitalWrite(pin, initial_state);
    }
    ~BlinkLed() {
      digitalWrite(pin, initial_state==HIGH ? LOW : HIGH);
    }

private:
  int pin, initial_state;
};
