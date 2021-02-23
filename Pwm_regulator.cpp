/*
 * Pwm_regulator
 * File:   Pwm_regulator.cpp
 * Author: Hirofumi Ikeda
 */
 
#include "Pwm_regulator.h"
// use first channel of 16 channels (started from zero)

uint8_t Pwm_regulator::timer_bit = 16;
int  Pwm_regulator::duty_max = 65535;
uint32_t Pwm_regulator::base_freq = 500;
float Pwm_regulator::a = 0.3746;
float Pwm_regulator::b = 10.0;

Pwm_regulator::Pwm_regulator(uint8_t pwm_ch, uint8_t pwm_pin, float T_current, float T_target ) {
  Pwm_regulator::pwm_ch=pwm_ch;
  Pwm_regulator::pwm_pin=pwm_pin;
  Pwm_regulator::T_target=T_target;
  Pwm_regulator::T_current=T_current;
  duty=0;
  delta_duty_ratio=0;
  duty_ratio = 0;  
  ledcSetup(pwm_ch, base_freq, timer_bit);
  ledcAttachPin(pwm_pin, pwm_ch);
  ledcWrite(pwm_ch, duty);
}

void Pwm_regulator::update(float T_current, float T_target, bool is_delta) {
  last_delta_duty_ratio = delta_duty_ratio;
  
  delta_duty_ratio = a * ( exp( (T_current - T_target)/b ) - 1.0 );  
  // prevent divergence 
  if (last_delta_duty_ratio*delta_duty_ratio < 0) delta_duty_ratio = delta_duty_ratio / 2.0;

  if (is_delta){
    duty = duty + duty_max * delta_duty_ratio;
  } else {
    if (delta_duty_ratio < 0.0) {
      delta_duty_ratio = 0.0;
    }
    if (delta_duty_ratio > 1.0) {
      delta_duty_ratio = 1.0;
    } 
    duty = duty_max * delta_duty_ratio;
  }
  Serial.begin(115200);
  delay(100);
//  delayMicroseconds(20000);
  Serial.println("> pwm_regulator");
  Serial.println("is_delta");
  Serial.println(is_delta);
  Serial.println("delta_duty_ratio");
  Serial.println(delta_duty_ratio);
  
  // prevent overflow
  if (duty > duty_max) {
    duty = duty_max;
  } else if (duty < 0) {
    duty = 0;      
  }
  
  duty_ratio = float(duty) / float(duty_max);

  ledcWrite(pwm_ch, duty);
}

void Pwm_regulator::pause() {
  ledcWrite(pwm_ch, 0);
}

void Pwm_regulator::resume() {
  ledcWrite(pwm_ch, duty);
}
