/*
 * File:   Ohakon.h
 * Author: Hirofumi Ikeda
 */
 
#ifndef __OHAKON_H__
#define __OHAKON_H__

#include "Arduino.h"
#include <WiFi.h>
#include <time.h>

class Ohakon {
  static int JST;
  static const char* ssid;
  static const char* password;
  static String endpoint;
  static String geom;

  public:
     Ohakon();
     bool sync();
     float sunrise;
     float sunset;
     float sunrise_tomorrow;
     float sunset_tomorrow;
     bool synchronized_before_sunrise;
     byte sync_second;
}; 
#endif
