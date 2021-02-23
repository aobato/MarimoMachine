/*
 * Aquarium
 * File:   Ohakon.cpp
 * Author: Hirofumi Ikeda
 */
 
#include "Ohakon.h"
#include <HTTPClient.h>

int Ohakon::JST = 3600* 9;
const char* Ohakon::ssid = "elecom2g-16A454";
const char* Ohakon::password = "7486942110208";
String Ohakon::endpoint = "http://labs.bitmeister.jp/ohakon/api/?mode=sun_rise_set";
String Ohakon::geom = "&lat=43.4572585&lng=144.1040147&days=2";

Ohakon::Ohakon() {
}

bool Ohakon::sync(){
  bool success;
  int loop_count=0;
  //WiFi set up
  Serial.begin(115200);  
  delay(10);
  Serial.println(ssid);
  Serial.println(password);  
  WiFi.begin(ssid, password);
  delay(1000);
  loop_count = 0;
  while(WiFi.status() != WL_CONNECTED) {
    delayMicroseconds(50000);
    Serial.print(".");
    loop_count++;
    if (loop_count > 20) {
      success = false;
      return success;
    }
  }
  Serial.println("connect OK!");

  // Configure internal time with NTP 
  configTzTime( "JST-9", "ntp.nict.jp", "ntp.jst.mfeed.ad.jp");

  // Get the date of today 
  time_t t;
  struct tm *tm;
  Serial.println("Sync start ");
  while(1) {  // time() return incorrect value until finish sync. 
    t = time(NULL);
    tm = localtime(&t);
    if (tm->tm_year != 70) {
      break;
    } else {
      delayMicroseconds(500000);
      Serial.print(".");
    }
  }
  Serial.println("Sync OK");
  int year = tm->tm_year+1900;
  int month = tm->tm_mon+1;
  int day = tm->tm_mday;
  int hr = tm->tm_hour;
  int mn = tm->tm_min;
  int sc = tm->tm_sec; 

  Ohakon::sync_second=sc; // keep for the starting value of the timer count
  
  Serial.println(year);
  Serial.println(month);
  Serial.println(day);  
  Serial.println(hr);  
  Serial.println(mn);  
  Serial.println(sc);  
  
  // Sun-set and Sun-rise time
  HTTPClient http;
  String today_string = "&year="+String(year)+"&month="+String(month)+"&day="+String(day);  
  http.begin(endpoint + today_string + geom); //URLを指定
  int httpCode = http.GET();  //GETリクエストを送信
 
  if (httpCode > 0) { //返答がある場合
    success = true;

    String payload = http.getString();  //返答（XML形式）を取得
    Serial.println(httpCode);
    Serial.println(payload);

    // Extract today's sun rise
    unsigned int i = payload.indexOf("<rise_and_set date_off=\'0\'>",0)+27;
    unsigned int start_index = payload.indexOf("<sunrise>",i)+9;
    unsigned int end_index = payload.indexOf("</sunrise>",i)-1;
    sunrise = payload.substring(start_index,end_index).toFloat();
    Serial.println(sunrise);
    
    // Extract today's sun set
    start_index = payload.indexOf("<sunset>",i)+8;
    end_index = payload.indexOf("</sunset>",i)-1;
    sunset = payload.substring(start_index,end_index).toFloat();

    // Extract tomorrow's sun rise
    i = payload.indexOf("<rise_and_set date_off=\'1\'>",0)+27;
    start_index = payload.indexOf("<sunrise>",i)+9;
    end_index = payload.indexOf("</sunrise>",i)-1;
    sunrise_tomorrow = payload.substring(start_index,end_index).toFloat();

    // Extract tomorrow's sun sey
    start_index = payload.indexOf("<sunset>",i)+8;
    end_index = payload.indexOf("</sunset>",i)-1;
    sunset_tomorrow = payload.substring(start_index,end_index).toFloat();

    // did synchronize before sunrise ?
    if (hr+mn/60.0 < sunrise) {
      synchronized_before_sunrise = true;
    } else {
      synchronized_before_sunrise = false;      
    }
    
  } else {
    Serial.println("Error in httpCode");      
    Serial.println(httpCode);
    success = false;
  }
  http.end(); //Free the resources  

  WiFi.mode(WIFI_OFF); //Save power
  
  return success;
}
