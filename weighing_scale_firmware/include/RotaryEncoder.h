#ifndef ROTARYENCODER_H
#define ROTARYENCODER_H

#include <Arduino.h>

class RotaryEncoder {
  private:
    byte sck;
    byte dt;
    byte sw;
    bool state;
    bool lastCLK;
    bool lastSW;
    unsigned long lastDebounceTime = 0;
    unsigned long debounceDelay = 100;
    //volatile unsigned long activeTime = 0;
    //volatile bool active = 1;
    // static volatile bool dir;

  public:
    RotaryEncoder(byte sck, byte dt, byte sw);
    void init();
    void updateSW();
    int dir();
    bool isPressed();
    //bool getState();
    // static void IRAM_ATTR SCK_ISR();
    // static void IRAM_ATTR SW_ISR();
    //int interval(bool active = 0);
    //unsigned long getActTime();
};

#endif
