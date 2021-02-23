/*
 * File:   TempS5851A.h
 * Author: Hirofumi Ikeda
 */
 
#ifndef __TEMPS5851A_H__
#define __TEMPS5851A_H__
  
#include "Arduino.h"
#include <S5851A.h>

class TempS5851A : private S5851A {
  static int nmx;
  public:
    uint8_t address;
    TempS5851A(uint8_t address);
    void init();
    float read();   
  private:
    void asc_sort(float readings[]);
};
#endif 
