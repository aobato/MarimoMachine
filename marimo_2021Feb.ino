#include "Aquarium.h"
#include "Ohakon.h"
#include "Radiator.h"
#include "Dashboard_data.h"
#include "time.h"
#include <Wire.h>  
#include "SSD1306Wire.h"        // managerで ssd1306 esp32　で検索
#include "Meteocons.h"
#include "WiFi.h"
#define daylight_LED_pin 14

int elapsed_min_since_last_sync=0;
bool succeed_sync_today;
char buff[24]="";
SSD1306Wire oled(0x3c, SDA, SCL);   // ADDRESS, SDA, SCL  -  SDA and SCL usually populate automatically based on your board's pins_arduino.h

void dashboard_draw_frame();
void dashboard_update(Ohakon &oha, Radiator &rd, Aquarium &aq, bool succeed_sync_today);

// Initilize user objects 
Aquarium aq;
Radiator rd;
Ohakon oha;

// タイマー割込み用の変数定義 
volatile int interruptCounter; /* 割り込みカウンタ */
hw_timer_t *timer1 = NULL; /* 割り込み制御用構造体 */
portMUX_TYPE timerMux1=portMUX_INITIALIZER_UNLOCKED; 

// ========================================================================
//                          FUNCTIONS
// ========================================================================
void IRAM_ATTR timerInterrupt() {
  portENTER_CRITICAL_ISR(&timerMux1);
  interruptCounter++;
  portEXIT_CRITICAL_ISR(&timerMux1);
}

// -------------------- DISPLAY --------------------------
void dashboard_print(String str){
  oled.clear();
  oled.setFont(ArialMT_Plain_10);
  oled.drawString(0,0,str);
  oled.display();
}
void dashboard_draw_frame(){
  uint16_t x0,y0,x1,y1,r,q;
  byte i,j;  

  // draw vertical lines
  j = pgm_read_byte_near(&vertical_line);
  for (byte i = 1; i <= j*4; i=i+4){
    x0 = pgm_read_byte_near(&vertical_line[i]);
    y0 = pgm_read_byte_near(&vertical_line[i+1] )-1;
    x1 = pgm_read_byte_near(&vertical_line[i+2] );
    y1 = pgm_read_byte_near(&vertical_line[i+3] )-1;
    oled.drawLine(x0,y0,x1,y1);
  }

  // draw fixed characters
  oled.setFont(ArialMT_Plain_10);
  oled.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
  oled.drawString(20,6,"TIME");
  oled.drawString(66,6,"BOX");
  oled.drawString(110,6,"ROOM");
  oled.setFont(Meteocons_Regular_10); 
  oled.drawString(80,15,"*");
  oled.setFont(ArialMT_Plain_10);
  oled.drawString(79,42,"W");

  oled.setFont(Meteocons_Regular_10); 
  oled.drawString(123,17,"*");
  oled.setFont(ArialMT_Plain_10);
  oled.drawString(123,34,"%");
  oled.drawString(119,48,"hPa");
  oled.display();
}

void dashboard_init(){
  oled.init();    //ディスプレイを初期化
  oled.flipScreenVertically();
}
void dashboard_update(Ohakon *oha, Radiator *rd, Aquarium *aq, bool succeed_sync_today){
  int sunrise_hr,sunrise_mn,sunset_hr,sunset_mn;
  int sunrise_tomorrow_hr,sunrise_tomorrow_mn,sunset_tomorrow_hr,sunset_tomorrow_mn;
  float T_room,H_room,P_room;
  float T_aqua,power;
  char buf[24];

  // sunrise/set  
  sunrise_hr = int(oha->sunrise);
  sunrise_mn = (oha->sunrise - sunrise_hr)*60.0;
  sunset_hr = int(oha->sunset);
  sunset_mn = (oha->sunset - sunset_hr)*60.0;
  sunrise_tomorrow_hr = int(oha->sunrise_tomorrow);
  sunrise_tomorrow_mn = (oha->sunrise_tomorrow - sunrise_tomorrow_hr)*60.0;
  sunset_tomorrow_hr = int(oha->sunset_tomorrow);
  sunset_tomorrow_mn = (oha->sunset_tomorrow - sunset_tomorrow_hr)*60.0;

  // environments
  T_room = rd->T_target;
  H_room = rd->H_current;
  P_room = rd->P_current;     
  
  // aquarium
  T_aqua = aq->T_current;
  power = aq->power;

  // Get the datetime of now 
  const char monthArrayName[][4] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul","Aug","Sep","Oct","Nev","Dec"};
  time_t t;
  struct tm *tm;
  t = time(NULL);
  tm = localtime(&t);
  byte month = tm->tm_mon;
  byte day = tm->tm_mday;
  byte hr = tm->tm_hour;
  byte mn = tm->tm_min;

  // clear all and draw flame
  oled.clear();
  dashboard_draw_frame();

  // date time
  oled.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
  oled.setFont(ArialMT_Plain_16);   
  sprintf(buf,"%02d:%02d",hr,mn);
  oled.drawString(19,20,buf);
  oled.setFont(ArialMT_Plain_10);  
  sprintf(buf,"%s-%2d",monthArrayName[month],day);
  oled.drawString(19,32,buf);

  // show fail to sync today
  if (not(succeed_sync_today)) {
    oled.setFont(ArialMT_Plain_10);
    oled.drawString(0,46,"!");
    oled.drawString(0,57,"!");
  }
  
  // sun set / sun rise
  if (hr+mn/60.0 < oha->sunrise) { // now is before sun-rise
    oled.setFont(ArialMT_Plain_10);
    sprintf(buf,"%2d:%02d",sunrise_hr,sunrise_mn);
    oled.drawString(27,46,buf);
    oled.setFont(Meteocons_Regular_10); 
    oled.drawString(8,46,"A");

    oled.setFont(ArialMT_Plain_10);
    sprintf(buf,"%2d:%02d",sunset_hr,sunset_mn);
    oled.drawString(27,57,buf);
    oled.setFont(Meteocons_Regular_10); 
    oled.drawString(8,57,"C");
        
  } else if (hr+mn/60.0 < oha->sunset) { // now is daylight
    oled.setFont(ArialMT_Plain_10);
    sprintf(buf,"%2d:%02d",sunset_hr,sunset_mn);
    oled.drawString(27,46,buf);
    oled.setFont(Meteocons_Regular_10); 
    oled.drawString(8,46,"C");

    oled.setFont(ArialMT_Plain_10);
    sprintf(buf,"%2d:%02d",sunrise_tomorrow_hr,sunrise_tomorrow_mn);
    oled.drawString(27,57,buf);
    oled.setFont(Meteocons_Regular_10); 
    oled.drawString(8,57,"A");

  } else { // now is after sun-set
    oled.setFont(ArialMT_Plain_10);
    sprintf(buf,"%2d:%02d",sunrise_tomorrow_hr,sunrise_tomorrow_mn);
    oled.drawString(27,46,buf);
    oled.setFont(Meteocons_Regular_10); 
    oled.drawString(8,46,"A");
    
    oled.setFont(ArialMT_Plain_10);
    sprintf(buf,"%2d:%02d",sunset_tomorrow_hr,sunset_tomorrow_mn);
    oled.drawString(27,57,buf);
    oled.setFont(Meteocons_Regular_10); 
    oled.drawString(9,57,"C");
  }

  // aquarium temperature
  oled.setFont(ArialMT_Plain_16);
  sprintf(buf,"%04.1f",T_aqua);
  oled.drawString(64,25,buf);

  // pertier power
  oled.setFont(ArialMT_Plain_16);
  sprintf(buf,"%3.1f",power);
  oled.drawString(64,51,buf);

  // room temperature
  oled.setFont(ArialMT_Plain_10);
  sprintf(buf,"%04.1f",T_room);
  oled.drawString(108,20,buf);

  // room humidity
  sprintf(buf,"H %02d",int(H_room));
  oled.drawString(108,36,buf);

  // room barometer
  sprintf(buf,"%04d",int(P_room));
  oled.drawString(110,58,buf);
    
  // display all
  oled.display();
}

// ========================================================================
//                          MAIN
// ========================================================================
void setup() {
  Serial.begin(115200);
  // setup pinmode  
  pinMode(daylight_LED_pin,OUTPUT);
   
  // initialize timer
  timer1 = timerBegin(0, 80, true); // タイマー0,プリスケーラ80,インクリメント
  timerAttachInterrupt(timer1, &timerInterrupt, true);
  timerAlarmWrite(timer1, 1000000, true); // 1sec間隔の割込

  // Maintenace 
  dashboard_init();
  dashboard_print("Start");

  aq.init();
  dashboard_print("aq.init");
  Serial.begin(115200);
  Serial.println("aq init OK!");
  aq.maintain();
  dashboard_print("aq.maintain");
  Serial.begin(115200);
  Serial.println("aq maintain OK!");
  rd.init();
  Serial.begin(115200);
  Serial.println("rd init OK!");
  rd.maintain();
  Serial.begin(115200);
  Serial.println("rd maintain OK!");

  // Syncronize the time using NTP and get sun rise/set time
  aq.pause_cooler();
  succeed_sync_today = oha.sync();
  if (not(succeed_sync_today)){
    Serial.println("Sync error!");
//    while(true){}
  } else {
    Serial.println("Sync OK!");    
  }
  aq.resume_cooler();
  
  // clear time counter
  portENTER_CRITICAL(&timerMux1);
  interruptCounter = int(oha.sync_second);
  portEXIT_CRITICAL(&timerMux1);

  // start timer
  timerAlarmEnable(timer1); 

  // display status
  dashboard_init();
  dashboard_update(&oha, &rd, &aq, succeed_sync_today);
}

void loop() {
  time_t t;
  struct tm *tm;
  byte hr,mn;
  
  // Wait for 1 minute 
  Serial.begin(115200);
  while (interruptCounter < 60) { 
  }
  Serial.println("work start");

  // clear counter
  portENTER_CRITICAL(&timerMux1);
  interruptCounter = 0;
  portEXIT_CRITICAL(&timerMux1);

  // Get current datetime
  t = time(NULL);
  tm = localtime(&t);
  hr = tm->tm_hour;
  mn = tm->tm_min;

  // Dispay date time 
  dashboard_init();
  dashboard_update(&oha, &rd, &aq, succeed_sync_today);
  
  //Manage Strirring flag
  if ((mn % 15 == 0) and (hr >= 8) and (hr <= 22)) aq.need_stirr = true; 
  
  // Manage LED daylight
  if (oha.synchronized_before_sunrise) { // Next sunrise will be occur within today
    if (hr+mn/60.0 < oha.sunrise or hr+mn/60.0 >= oha.sunset ) {
      digitalWrite(daylight_LED_pin,LOW);
    } else {
      digitalWrite(daylight_LED_pin,HIGH); 
    }
  } else { // Next sunrise  will be occur in tomorrow
    if (hr+mn/60.0 >= oha.sunset or hr+mn/60.0 < oha.sunrise_tomorrow) {
      digitalWrite(daylight_LED_pin,LOW);
    } else {
      digitalWrite(daylight_LED_pin,HIGH);
    } 
  }
  
  // Maintenace 
  rd.maintain();
  aq.maintain();
  
  // Dispay sensor data 
  dashboard_init();
  dashboard_update(&oha, &rd, &aq, succeed_sync_today);
  
  // Synchronize the clock time only once after sun-rise + 1 hour
  if (not(succeed_sync_today) and hr+mn/60.0 > oha.sunrise_tomorrow + 1.0) {
    aq.pause_cooler();  // prevent too many current flow by wifi power
    succeed_sync_today = oha.sync();
    WiFi.mode(WIFI_STA);
    aq.resume_cooler();
  }

  // reset sync flg at 0:01
  if (hr==0 and mn==1 and succeed_sync_today) succeed_sync_today = false;

  // clear stirr flag
  aq.need_stirr = false;
}
//---------------------------------------------------------------
//-------------------------------------------------------
