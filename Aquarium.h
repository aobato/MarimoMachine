/*
 * File:   Pwm_regulator.h
 * Author: Hirofumi Ikeda
 */
 
#ifndef __AQUARIUM_H__
#define __AQUARIUM_H__

#include "Arduino.h"
#include "Pwm_regulator.h"
#include "TempDS18B20.h"
#include "Stirring.h"

class Aquarium : private TempDS18B20, private Pwm_regulator, private Stirring {
  public:
     Aquarium();
     void init();
     void maintain();
     void pause_cooler();
     void resume_cooler();     
     float T_current;
     bool need_stirr;
     float power;
     float duty_ratio;
     static uint8_t peltier_pin;
     static uint8_t peltier_pwm_ch;
     static uint8_t stirring_motor_pin1;
     static uint8_t stirring_motor_pin2;
     static uint8_t stirring_pwm_ch;
     static float T_target;
     static uint8_t sensor_pin;
  private:
     float T_last;
};
#endif /* __AQUARIUM__ */
