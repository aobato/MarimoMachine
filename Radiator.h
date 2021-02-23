/*
 * File:   Radiator.h
 * Author: Hirofumi Ikeda
 */
 
#ifndef __RADIATOR_H__
#define __RADIATOR_H__

#include "Arduino.h"
#include "Pwm_regulator.h"
#include "Seeed_BME280.h"

#include "TempS5851A.h"

class Radiator {
  public:
     static uint8_t fan_pwm_pin;
     static uint8_t fan_power_pin;
     static uint8_t fan_pwm_ch;
     static uint8_t S5851A_address;
     Radiator();
     void init();
     void maintain();
     float T_current;
     float T_target;
     float H_current;
     float P_current; 
     float duty_ratio;          
}; 
#endif 
