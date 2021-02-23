/*
 * File:   Pwm_regulator.h
 * Author: Hirofumi Ikeda
 */
 
#ifndef __PWM_REGULATOR_H__
#define __PWM_REGULATOR_H__
 
#include "Arduino.h"

class Pwm_regulator {
  static uint8_t timer_bit;
  static int  duty_max;
  static uint32_t base_freq;
  static float a;
  static float b;

  public:
     Pwm_regulator(uint8_t  pwm_ch, uint8_t pwm_pin, float T_current, float T_target);
     float T_current;
     float T_target;
     int duty;
     float duty_ratio;
     byte pwm_ch,pwm_pin;
     void update(float T_current, float T_target, bool is_delta);
     void pause();
     void resume();     
  private:
     float delta_duty_ratio;
     float last_delta_duty_ratio;
}; 
#endif /* __PWM_REGULATOR_H__ */
