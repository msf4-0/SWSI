#ifndef MY_LED_H
#define MY_LED_H
#include <Arduino.h>
class Led {

  private:
    byte pin;

  public:
    Led(byte pin);
    void start(byte pin);
    void init();
    void on();
    void off();
};
#endif
