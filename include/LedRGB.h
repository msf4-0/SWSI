#ifndef LEDRGB_H
#define LEDRGB_H

#include <Arduino.h>

class LedRGB {
  private:
    byte rPin;
    byte gPin;
    byte bPin;
    byte rChan;
    byte gChan;
    byte bChan;
    int res;
    int freq;
    float duty;

  public:
    // Constructor
    LedRGB(byte rPin, byte gPin, byte bPin, int res, int freq, byte rChan, byte bChan, byte gChan, float duty);

    void init();
    void color(const char* color);
    void rgb(byte r, byte g, byte b);
    void onPWM(byte pin, byte val);
    void on(byte pin);
    void off(byte pin);
};
#endif
