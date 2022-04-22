#include "RotaryEncoder.h"

RotaryEncoder::RotaryEncoder(byte sck, byte dt, byte sw) {
  this->sck = sck;
  this->dt = dt;
  this->sw = sw;
  lastCLK = 0;
  lastSW = 1;
  init();
}
void RotaryEncoder::init() {

  pinMode(sck, INPUT);
  pinMode(sw, INPUT_PULLUP);
  pinMode(dt, INPUT);
  // attachInterrupt(sck, SCK_ISR, CHANGE);
  // attachInterrupt(sw, SW_ISR, FALLING);
  updateSW();
}

int RotaryEncoder::dir() {
    bool curCLK = digitalRead(sck);
    int dir;

    if (curCLK != lastCLK) {
      if (digitalRead(dt)!= curCLK)  // CCW
        dir = 1;
      else  //CW
        dir = 2;
    }
    else
      dir = 0;
    lastCLK = curCLK;
    return dir;
}
void RotaryEncoder::updateSW() {
    state = 0;
    bool newSW = digitalRead(sw);

    if (newSW != lastSW) lastDebounceTime = millis();
    if (millis() - lastDebounceTime > debounceDelay && newSW == 0) state = 1;

    lastSW = newSW;
}

bool RotaryEncoder::isPressed() {
  updateSW();
  return state;
}
