/*
 * Aquarium
 * File:   Aquarium.cpp
 * Author: Hirofumi Ikeda
 */
 
#include "Aquarium.h"
#include "Pwm_regulator.h"
#include "Stirring.h"
#include "TempDS18B20.h"

uint8_t Aquarium::peltier_pin = 23;
uint8_t Aquarium::peltier_pwm_ch = 0;
uint8_t Aquarium::stirring_motor_pin1 = 16;
uint8_t Aquarium::stirring_motor_pin2 = 17;
uint8_t Aquarium::stirring_pwm_ch = 1;
uint8_t Aquarium::sensor_pin = 13;

float Aquarium::T_target = 20.0;

Aquarium::Aquarium():TempDS18B20(Aquarium::sensor_pin),Pwm_regulator(),
  Stirring(stirring_pwm_ch,stirring_motor_pin1,stirring_motor_pin2) {
  need_stirr=false;
}

void Aquarium::init(){
  T_current = TempDS18B20::read();
  Pwm_regulator::init(peltier_pwm_ch, peltier_pin, T_current, T_target);
}

void Aquarium::maintain(){
  if (need_stirr) {
    Pwm_regulator::pause();
    Stirring::on();
    Pwm_regulator::resume();
  }
  T_last = T_current;
  T_current = TempDS18B20::read();
  delay(100);
  Pwm_regulator::update(T_current,T_target,true);
  power = Pwm_regulator::duty_ratio * 3.3 * 3.3 / 2.65;
  duty_ratio=Pwm_regulator::duty_ratio;
}
void Aquarium::pause_cooler(){
  Pwm_regulator::pause();  
}
void Aquarium::resume_cooler(){
  Pwm_regulator::resume();  
}
