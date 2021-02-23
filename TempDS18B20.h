/*
 * File:   TempDS18B20.h
 * Author: Hirofumi Ikeda
 */
 
#ifndef __TEMPDS18B20_H__
#define __TEMPDS18B20_H__
 
#include "Arduino.h"
#include <OneWire.h>
 
class TempDS18B20 : private OneWire{
  static int nmx;
    int pin;
  public:
    TempDS18B20(byte pin);
    float read();
};
#endif 
