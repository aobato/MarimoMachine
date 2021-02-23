/*
 * File:   stirring.h
 * Author: Hirofumi Ikeda
 */
 
#ifndef __STIRRING_H__
#define __STIRRING_H__
 
#include "Arduino.h"
 
class Stirring {
  static int timer_bit;
  static int base_freq;
  static int init_duty;   
  static int base_duty;

  public:
    Stirring(uint8_t pwm_ch, uint8_t motor_pin1, uint8_t motor_pin2);
    void on();
    
  private:
    uint8_t pwm_ch;

}; 
#endif /* __STIRRING_H__ */
