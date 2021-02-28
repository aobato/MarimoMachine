/*
 * Stirring 
 * File:   TempS5851A.cpp
 * Author: Hirofumi Ikeda
 */
 
#include "TempS5851A.h"
#include <Wire.h>

int TempS5851A::nmx = 20;

TempS5851A::TempS5851A(uint8_t address):S5851A(address) {
  TempS5851A::address = address;
}

void TempS5851A::init(){
  Serial.begin(115200);
//  Wire.begin();
  // Reset all connected S-5851A sensor by I2C General Call Address.
  // Refer to "8. General call" on datasheet.
  delay(100);
  S5851A::resetByGeneralCall();
/*
  while(S5851A::resetByGeneralCall()) {
    Serial.println("failed: reset by general call in Address TempS5851A.cpp");
    Serial.println(address);    
    delay(100);
//  if (!S5851A::resetByGeneralCall()) {
//    Serial.println("failed: reset by general call in Address TempS5851A.cpp");
//    Serial.println(address);
//    while(1) {
//      delay(100);
//    };
  };
  Serial.println("S5851A initialized");
  delay(100);
*/
}


float TempS5851A::read(){
  float readings[nmx];
  float median;
  Serial.begin(115200);
  Serial.println("S5851A read start");
  for (int i=0;i<nmx;i++) {
    S5851A::update();
    readings[i] = S5851A::getTempC(); 
/*
    Serial.println(readings[i]);
*/
    delay(50);
  }
  asc_sort(readings);
  median = readings[nmx/2];
/*
  Serial.println("median");
  Serial.println(median);
*/
  return median;
}

void TempS5851A::asc_sort(float a[]) {
  for(int i=0; i<(nmx-1); i++) {
    for(int o=0; o<(nmx-(i+1)); o++) {
      if(a[o] > a[o+1]) {
        int t = a[o];
        a[o] = a[o+1];
        a[o+1] = t;
      }
    }
  }
}
