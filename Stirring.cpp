/*
 * Stirring 
 * File:   stirring.cpp
 * Author: Hirofumi Ikeda
 */
 
#include "Stirring.h"

int Stirring::timer_bit = 16;
int Stirring::base_freq = 500;
int Stirring::init_duty = 61680;   
int Stirring::base_duty = 17990;

Stirring::Stirring(uint8_t pwm_ch, uint8_t motor_pin1, uint8_t motor_pin2) {
  Stirring::pwm_ch=pwm_ch;
  ledcSetup(pwm_ch, base_freq, timer_bit);
  ledcAttachPin(motor_pin1, pwm_ch);
  pinMode(motor_pin2,OUTPUT);
  digitalWrite(motor_pin2,LOW);
  ledcWrite(pwm_ch, 0);
}

void Stirring::on(){
  // start stirring 
  ledcWrite(pwm_ch, init_duty);
  delay(200);
  ledcWrite(pwm_ch, base_duty);
  delay(1200);
  ledcWrite(pwm_ch, 0);
}
