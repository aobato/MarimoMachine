/*
 * Radiator
 * File:   Radiator.cpp
 * Author: Hirofumi Ikeda
 */
 
#include "Radiator.h"
#include "Pwm_regulator.h"
#include "TempS5851A.h"
#include "Seeed_BME280.h"
#include <Wire.h>

uint8_t Radiator::fan_pwm_pin = 19;
uint8_t Radiator::fan_power_pin = 32;
uint8_t Radiator::fan_pwm_ch = 2;
uint8_t Radiator::S5851A_address = 0x4A;
BME280 bme280;
TempS5851A radiator_temp(Radiator::S5851A_address);
//TempS5851A radiator_temp(0x4A);
Pwm_regulator fan(Radiator::fan_pwm_ch, Radiator::fan_pwm_pin, 25.0, 25.0);

Radiator::Radiator() {
}

void Radiator::init(){
  Serial.begin(115200);
  radiator_temp.init();
  if(!bme280.init()){
    Serial.println("BME280 initialization error!");
  }
  // fan pwm on
  pinMode(fan_power_pin,OUTPUT);
  digitalWrite(fan_power_pin,HIGH);
}
void Radiator::maintain(){
  T_current = radiator_temp.read();
  T_target = bme280.getTemperature();
  H_current = bme280.getHumidity();
  P_current = bme280.getPressure()/100.0;
  duty_ratio = fan.duty_ratio;
  Serial.begin(115200);
  delay(100);
  Serial.println("T_target-T_current");
  Serial.println(T_target-T_current);
  fan.update(T_current,T_target,false);  
}
