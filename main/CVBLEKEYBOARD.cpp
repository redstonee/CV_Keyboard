/*******include*********/
#include <Hid2Ble.h>
#include <BLEServer.h>
#include "BLE2902.h"
#include "BLECharacteristic.h"
#include <Adafruit_NeoPixel.h>
/*******define*********/
#define keyx digitalRead(9)
#define keyc digitalRead(8)
#define keyv digitalRead(4)
#define PIXEL_PIN    5 
#define PIXEL_COUNT 3  // 灯珠的数量
#define BRIGHTNESS 100
/****************/
Hid2Ble hid2Ble; //BLE
hw_timer_t * timer = NULL;//声明定时器
Adafruit_NeoPixel strip(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

/*******函数声明*********/
void send_ARR();
void updateHSV();
void LED_Breathe(uint8_t num);
void LED_DISPLAY();
void RGB_breathe();
void breathe_time();
void Breathe_all(uint8_t LED_NUM);
void LED_DISPLAY(uint8_t effet_type);
/*******RGB部分*********/
int lasttime[2];
uint8_t Effet_type=1;
struct RGB_LED
{
   uint8_t enable=0;
   uint8_t bright=250;
   uint8_t r,g,b;
   int last_time;  
} LED[3];




/*******键盘部分*********/
char ctrlc[5] = {0x01,0x00,0x06, 0x00, 0x00}; //ctrl+x
char ctrlx[5] = {0x01, 0x00, 0x1b, 0x00, 0x00};//ctrl+c
char ctrlv[5] = {0x01, 0x00, 0x19, 0x00, 0x00};//ctrl+v
char lockArray[5] = {0x00, 0x00, 0x00, 0x00, 0x00};//Key UP
uint8_t enablec=0;
uint8_t enablex=0;
uint8_t enablev=0;
uint8_t up[3]={0};
uint8_t enablelock=0;
class Mycallback : public BLECharacteristicCallbacks
{

};
/****************/
void IRAM_ATTR onTimer() { //Interrupt Service Function: Detect Key
  if(up[2]==1){
    if(keyx) {enablelock=1;up[2]=0;LED[2].last_time=millis();}//Key UP
    }
   else{  if(keyx==0) {enablex=1;}//Key Ctrl+X DOWN
   }
  if(up[1]==1){
    if(keyc) {enablelock=1;up[1]=0;LED[1].last_time=millis();}
    }
   else{  if(keyc==0) {enablec=1;}
   }
  if(up[0]==1){
    if(keyv) {enablelock=1;up[0]=0;LED[0].last_time=millis();}
    }
   else{  if(keyv==0) {enablev=1;}
   }



}

/*******键盘部分*********/
void setup()
{
  Serial.begin(115200);
  pinMode(9,INPUT_PULLUP);//enable GPIO
  pinMode(8,INPUT_PULLUP);//enable GPIO
  pinMode(4,INPUT_PULLUP);//enable GPIO

  strip.begin(); // Initialize NeoPixel strip object (REQUIRED)
  strip.clear();
  strip.setBrightness(BRIGHTNESS);
  strip.show();

  //Enable Timer and Interrupt Controller
  timer = timerBegin(0, 80, true);                
  timerAttachInterrupt(timer, &onTimer, true);   
  timerAlarmWrite(timer, 10000, true); // 10ms  
  timerAlarmEnable(timer);
  
  //Start BLE
  hid2Ble.setCallBack(new Mycallback());
  hid2Ble.begin();
  lasttime[0]=millis();
  lasttime[1]=millis();
}  

void loop()
{
  
 if (hid2Ble.isConnected())// if BLE Connected
 {
   send_ARR();//send Pushdata
 }
LED_DISPLAY(Effet_type);
if(millis()-lasttime[0]>=10){
   strip.show();
   lasttime[0]=millis();
  }
 if(millis()-lasttime[1]>=200){
   if(!(keyx||keyc||keyv)){//change led effect
   Effet_type++;
  if(Effet_type>2) Effet_type=1;
   }
   lasttime[1]=millis();
  } 
 

  
}

void send_ARR(){
   if(enablex&&(up[2]==0)) { //push ctrl+x
    hid2Ble.send2Ble(ctrlx);
    up[2]=1;
    enablex=0;
     LED[2].enable=1;
    
    }
    if(enablec&&(up[1]==0)) { //push ctrl+c
    hid2Ble.send2Ble(ctrlc);
    up[1]=1;
    enablec=0;
     LED[1].enable=1;

    }
    if(enablev&&(up[0]==0)) { //push ctrl+v
    hid2Ble.send2Ble(ctrlv);
    up[0]=1;
    enablev=0;
     LED[0].enable=1;
  
    }
    if(enablelock){        //Key UP
     hid2Ble.send2Ble(lockArray);
     enablelock=0;
    }
  }
  


void LED_DISPLAY(uint8_t effet_type){
 switch(effet_type){
  case 1://

 if(LED[2].enable){
    LED_Breathe(2);
 }
 else strip.setPixelColor(2,0, 0,0);
 if(LED[1].enable){
    LED_Breathe(1);
 }
 else strip.setPixelColor(1,0, 0,0);
  if(LED[0].enable){
    LED_Breathe(0);
 }
 else strip.setPixelColor(0,0, 0,0);
 
 break;
  case 2:
  if(LED[0].enable)  Breathe_all(0);
  if(LED[1].enable)  Breathe_all(1);
  if(LED[2].enable)  Breathe_all(2);
  if((LED[0].enable==0)&&(LED[1].enable==0)&&(LED[2].enable==0))
  for(int i=0;i<3;i++)strip.setPixelColor(i,0, 0,0);
   break;
  
 }
  }
void LED_Breathe(uint8_t num){
  switch(num){
   case 0: 
     LED[0].r=0;
     LED[0].g=10;
     LED[0].b=5+LED[0].bright;
     breathe_time();
     strip.setPixelColor(0,LED[0].r, LED[0].g,LED[0].b);
     break;
   case 1:
     LED[1].r=0;
     LED[1].g=10;
     LED[1].b=5+LED[1].bright;
     breathe_time();
     strip.setPixelColor(1,LED[1].r, LED[1].g,LED[1].b);
     break;
   case 2:  
     LED[2].r=0;
     LED[2].g=10;
     LED[2].b=5+LED[2].bright;
     breathe_time();
     strip.setPixelColor(2,LED[2].r, LED[2].g,LED[2].b);
      break;
  }

  
  }
  void breathe_time(){
    if(LED[0].enable&&(up[0]==0)){
      if((millis()-LED[0].last_time)>=1){
        LED[0].bright-=1;
        if(LED[0].bright<=20){ LED[0].bright=250;LED[0].enable=0;}
        LED[0].last_time=millis();
      }
    }
    if(LED[1].enable&&(up[1]==0)){
      if((millis()-LED[1].last_time)>=1){
        LED[1].bright-=1;
        if(LED[1].bright<=20){ LED[1].bright=250;LED[1].enable=0;}
        LED[1].last_time=millis();
      }
    }
    if(LED[2].enable&&(up[2]==0)){
      if((millis()-LED[2].last_time)>=1){
        LED[2].bright-=1;
        if(LED[2].bright<=20){ LED[2].bright=250;LED[2].enable=0;}
        LED[2].last_time=millis();
      }
    }
    }

void Breathe_all(uint8_t LED_NUM){
     LED[LED_NUM].r=0;
     LED[LED_NUM].g=5+LED[LED_NUM].bright;
     LED[LED_NUM].b=10;
     breathe_time();
  for(int i=0;i<3;i++)  strip.setPixelColor(i,LED[LED_NUM].r, LED[LED_NUM].g,LED[LED_NUM].b);

  }
