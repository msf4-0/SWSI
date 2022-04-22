#include "LedRGB.h"

LedRGB::LedRGB(byte rPin, byte bPin, byte gPin, int res, int freq, byte rChan, byte bChan, byte gChan, float duty) {
  this->rPin = rPin;
  this->gPin = gPin;
  this->bPin = bPin;
  this->rChan = rChan;
  this->gChan = gChan;
  this->bChan = bChan;
  this->res = res;
  this->freq = freq;
  this->duty = duty;
  init();
}

void LedRGB::init() {
  ledcSetup(rChan, freq, res);
  ledcSetup(gChan, freq, res);
  ledcSetup(bChan, freq, res);
  ledcAttachPin(rPin, rChan);
  ledcAttachPin(gPin, gChan);
  ledcAttachPin(bPin, bChan);
  off(rChan);
  off(gChan);
  off(bChan);
}

void LedRGB::color(const char* color) {
  if (strcmp(color,"clear")==0)
    rgb(0, 0, 0);
  else if (strcmp(color,"red")==0)
    rgb(255, 0, 0);
  else if (strcmp(color,"green")==0)
    rgb(0, 0, 255);
  else if (strcmp(color,"blue")==0)
    rgb(0, 255, 0);
  else if (strcmp(color,"orange")==0)
    rgb(255, 100, 0);
  else if (strcmp(color,"yellow")==0)
    rgb(255, 255, 0);
  else if (strcmp(color,"purple")==0)
    rgb(128, 0, 128);
  else if (strcmp(color,"cyan")==0)
    rgb(0, 255, 255);
  else if (strcmp(color,"magenta")==0)
    rgb(255, 0, 255);
}

void LedRGB::rgb(byte r, byte g, byte b) {
  onPWM(rChan, r);
  onPWM(gChan, g);
  onPWM(bChan, b);
}

// LED Pins should be current-sunked (cathode connected to pins)
void LedRGB::onPWM(byte pin, byte val) {
  const byte ledDuty = (byte) val*duty;
  ledcWrite(pin, 255 - ledDuty);
}

// On/off pin indivudally -> not used
void LedRGB::on(byte pin) {
  ledcWrite(pin, 0);
}

void LedRGB::off(byte pin) {
  ledcWrite(pin, 255);
}
