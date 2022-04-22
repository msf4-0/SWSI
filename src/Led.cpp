#include "Led.h"

Led::Led(byte pin) {
  this->pin = pin;
  init();
}
void Led::start(byte pin) {
  this->pin = pin;
  init();
}
void Led::init() {
  pinMode(pin, OUTPUT);
  // Always try to avoid duplicate code.
  // Instead of writing digitalWrite(pin, LOW) here,
  // call the function off() which already does that
  off();
}
void Led::on() {
  digitalWrite(pin, LOW);
}
void Led::off() {
  digitalWrite(pin, HIGH);
}
