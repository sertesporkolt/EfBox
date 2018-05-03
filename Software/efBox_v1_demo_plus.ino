

/*
 * ************************************************************************************
 * 
 *                      efBox_V1  driver program
 *                      
 * ***********************************************************************************
 * 
 * This is the second version of driver program of my efBox_V1, with demo.
 * 
 * 
 * 
 */





#include <Wire.h>
#include <ACROBOTIC_SSD1306.h> // Please install the Acrobotic OLED library via sketch->include library-> manage libraries before you compile/upload the program to NMCU :)
                               // I found, that the Acrobotic library is simple enough for my purposes( simple bitmap and character handling ). The two charactersize a bit 
                               // tight, but if you are an Arduino or C++ expert, you can create your own fonts :)
#include<ESP8266WiFi.h>                               
#include "images.h"            // Images what you need in hex form

/*---------------------------------------------------------------------------------------------------------


 --------------       PINOUT        ------------------------------------------------------------------------
 
                               HW    ->   SW

       NodeMCU I2C  SCK        D1    ->    5
       NodeMCU I2C  SDA        D2    ->    4
       upper Button            Tx0   ->    1
       lower Button            D7    ->   13
       reed sensor             D6    ->   12
       LED cooling fan         Rx0   ->    3
       LED panel               SD3   ->   10
       Step-up power module    D5    ->   14

------------------------------------------------------------------------------------------------------------

**************************   IMPORTANT  *********************
*   
*   You can choose of course another pin configuration,
*   I am sure if there is some better ( zb. if you make a PCB
*   probably my pinout is not optimal).
*   But better if you forget the D3(0),D4(2),D8(15) pins, or at least
*   you know exactly what you do, because the state of these pins
*   determine the startup mode of the chip, so wrong usage can
*   lead to surprise and pain... :/
*   
*   You find more about this e.g. here :
*   
*   https://www.forward.com.au/pfod/ESP8266/GPIOpins/index.html
*   
*   
*************************************************************
 
*/
 
const int buttonPlus=1;
const int buttonMinus=13;
const int led=10;
const int fan=3;
const int power=14;
const int reed=12;
const int beeper=9;

int timer=0;
int plusRead=HIGH;
int minusRead=HIGH;
int reedRead=HIGH;

unsigned long t = 0;


const char* ssid="your_WiFi_SSID";
const char* pass="your_WiFi_password";
String apiKey="your_thingspeak_Write_API_Key";
const char* server="api.thingspeak.com";

WiFiClient client;

void setup() {
 pinMode(buttonPlus, INPUT); 
 pinMode(buttonMinus, INPUT);
 pinMode(reed, INPUT);
 pinMode(power, OUTPUT);
 pinMode(led, OUTPUT);
 pinMode(fan, OUTPUT);
 pinMode(beeper, OUTPUT);
 
 digitalWrite(power,LOW);
 digitalWrite(led,LOW);
 digitalWrite(beeper,LOW);
 digitalWrite(fan,LOW);
 
Wire.begin();  

//---------------------------    splash screens, please let it untouched in next versions ( but feel free append yours )  :)))    ------------------------------

  oled.init();                      // Initialze SSD1306 OLED display

  
  oled.clearDisplay();              // Clear screen
  oled.drawBitmap(efbox_logo, 1024);   // 1024 pixels for logo

  delay(3000);

  oled.clearDisplay();              
  oled.drawBitmap(esef_logo, 1024);   
  delay(3000);

  oled.clearDisplay();              
  oled.drawBitmap(logor3, 1024);   

  delay(3000); 

//------------------------------------------- WiFi connect --------------------------------------------------------------------


  
 

WiFi.mode(WIFI_STA);
WiFi.begin(ssid,pass);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
   plusRead=digitalRead(buttonPlus);
  minusRead=digitalRead(buttonMinus); 
  if(plusRead==LOW || minusRead==LOW){
    break;
    }
}
delay(200);
  if(WiFi.status()==WL_CONNECTED){ 

 oled.clearDisplay();
 oled.setTextXY(2,2);               
 oled.putString("CONNECTED TO :");
 oled.setTextXY(5,2); 
 oled.putString("MQTT_Gateway_"); 
 oled.setTextXY(7,7); 
 oled.putString("001"); 

   delay(2000);
 
}

delay(1000);
// ------------------------------------    MAIN PROGRAM COMES  -----------------------------------------------------------------------

void setup();

loadOrDemo();
  
timerSetupFunc();

readyToRun();

gelRun();


 
 digitalWrite(power,LOW);
 digitalWrite(led,LOW);
 digitalWrite(beeper,LOW);
 digitalWrite(fan,LOW);

}



// ------------------------------------- in loop only the last steps repeats after the main program completed  ----------------------------------------

void loop() {
  plusRead=digitalRead(buttonPlus);
  minusRead=digitalRead(buttonMinus);
  if (minusRead==LOW || plusRead==LOW){
 lightOn();
  }
oled.clearDisplay();         
oled.drawBitmap(done, 1024);  
ESP.wdtFeed();                      

delay(2000);
 oled.clearDisplay();              
 oled.drawBitmap(efbox_logo, 1024);
   
plusRead=digitalRead(buttonPlus);
  minusRead=digitalRead(buttonMinus);
  if (minusRead==LOW || plusRead==LOW){
 lightOn();
  }
  
delay(2000);

digitalWrite(beeper,HIGH);
delay(50);
digitalWrite(beeper,LOW);
 
}

//-------------------------------------------       It's a demo part :)      -----------------------------------------------------------

void demo(){
  timer=60;
  tps();
  while(timer>0){

  for(int i=0;i<3;i++){
   for(int j=1;j<7;j++){
    switch(j){
     case 1 :
     oled.clearDisplay();             
     oled.drawBitmap(first, 1024);
     break;
     case 2 :
     oled.clearDisplay();             
     oled.drawBitmap(second, 1024);
     break;
     case 3 :
     oled.clearDisplay();             
     oled.drawBitmap(third, 1024);
     break;
     case 4 :
     oled.clearDisplay();             
     oled.drawBitmap(fourth, 1024);
     break;
     case 5 :
     oled.clearDisplay();             
     oled.drawBitmap(fifth, 1024);
     break;
     case 6 :
     oled.clearDisplay();             
     oled.drawBitmap(sixth, 1024);
     break;
    }
    for (int k=0;k<220;k++){
    analogWrite(led,k);
    delay(7);
    
  }
    
    for (int k=220;k>0;k--){
      analogWrite(led,k);
    delay(7);
    
  }
    }
    } 
    
    tps();
    timer--;
    if (timer<3){
      timer=60;
      }
  }
    
  }

//-------------------------------------------    30 sec lighting of LED panel -----------------------------------------------------------

// The cooling fan needs a bit more runtime(10s)after the LED panel down, for 100% safe cooling down. You can cut shorten this intervall or if you 
// have another lighting what doesn't need cooling you can completely outcomment this lines .

void lightOn(){

  oled.clearDisplay();             
  oled.drawBitmap(shining, 1024);   
  digitalWrite(power,LOW);
  digitalWrite(fan,HIGH);
  digitalWrite(led,HIGH);

  delay(30000);

  digitalWrite(led,LOW);

  delay(5000);

  digitalWrite(fan,LOW);
  
  }


//-----------------------------    It checks reed contact, if it is closed or not for safety. If you lift up the lid with magnet, the power modul shuts down immediately.... safety first :) -------------
  void reedCheck(){
  reedRead=digitalRead(reed);

  if (reedRead==HIGH){
     digitalWrite(power,LOW);
  oled.clearDisplay();             
  oled.drawBitmap(lid_open, 1024);   

    }
    
    while (reedRead==HIGH){
  delay(50);           // ESP.wdtFeed() is important in all cycles where you want only wait and polling. It feeds the software watchdog, if you do this not, in 3 secs resets the processor.
                              // Disable the soft wdt helps not, in this case starts the hardware watchdog immediately and resets in every 6 secs.... I've learned this through a bit sucks :/
  reedRead=digitalRead(reed);
  
   }
 digitalWrite(power,HIGH);
  
  }

 //----------------------------------------------    First menu item ---------------------------------------------------

  void loadOrDemo(){
    timer=0;
  
  oled.clearDisplay();           
  oled.drawBitmap(start, 1024);  

   
  
  plusRead=digitalRead(buttonPlus);
  minusRead=digitalRead(buttonMinus);

while (plusRead==HIGH){
  ESP.wdtFeed();
  plusRead=digitalRead(buttonPlus);
  minusRead=digitalRead(buttonMinus);
  if (minusRead==LOW){
 demo();
 
  }
 
}
delay(500);
  }

// ----------------------------------------------------     Second menu item   -------------------------------------------------------

void timerSetupFunc(){

  analogWrite(led,10);
  oled.clearDisplay();              
  oled.drawBitmap(timer_setup, 1024); 
  delay(500);
  plusRead=digitalRead(buttonPlus);
  minusRead=digitalRead(buttonMinus);
  while(plusRead==HIGH && minusRead==HIGH){
     ESP.wdtFeed();
    plusRead=digitalRead(buttonPlus);
  minusRead=digitalRead(buttonMinus);
    
     }

     delay(500);

 oled.clearDisplay();                 //  Acrobotic OLED lib. character handling, what you need to know the default font is 8x8.
                                     //  You can choose another font size, but the only is 5x7 .... so is not a really option on the 128x64 display ... :)
 oled.setTextXY(0,5);   
 oled.putString("TIMER :");          
 oled.setTextXY(4,8);                
 oled.putNumber(timer);
 oled.setTextXY(7,5); 
 oled.putString("MINUTES");        

 while (minusRead==HIGH){
  ESP.wdtFeed();
  plusRead=digitalRead(buttonPlus);
  minusRead=digitalRead(buttonMinus);
  if (plusRead==LOW && timer<=60){
    if(timer==60){
      timer=0;
      }
 timer=timer+5;
         
 oled.setTextXY(4,8);                
 oled.putString("  ");
 oled.setTextXY(4,8);                
 oled.putNumber(timer);
 
  }
  delay(150);
}

delay(500);
 
  }

 //-------------------------------    Menu item before you enter in the really important function ---------------------------------

  void readyToRun(){
    analogWrite(led,0);
  plusRead=digitalRead(buttonPlus);
  oled.clearDisplay();              // Clear screen
  oled.drawBitmap(gel_start, 1024);   // 1024 pixels for logo
    
  while (plusRead==HIGH){
  plusRead=digitalRead(buttonPlus);
  ESP.wdtFeed();
 }
 
}

//-------------------------------   Goodbye menu item, because good breeding is important :)))    ---------------------------------

void haveANice(){

timer=0;

  digitalWrite(power,LOW);
 digitalWrite(led,LOW);
 digitalWrite(beeper,LOW);
 digitalWrite(fan,LOW);

  oled.clearDisplay();              
  oled.drawBitmap(have_a_nice, 1024);  

  delay(2000);

  }


    //---------------------------------    This is the most important and main part ------------------------------------

void gelRun(){
    reedCheck();
  
   
   digitalWrite(power,HIGH);
   
// ----------------------------------    if the known wifi-network no available switch to offline mode -------------------

  if(WiFi.status()==WL_NO_SSID_AVAIL){ 

    offlineMode();
 
   }
   
//----------------------------------------------  normally enter to online mode   ------------------------------------------
 else{
   
oled.clearDisplay(); 
 oled.setTextXY(0,1); 
 oled.putString("DEVICE ONLINE");
 oled.setTextXY(3,2); 
 oled.putString("CHECK YOUR");
 oled.setTextXY(5,0); 
 oled.putString("CONNECTED PHONE");
 oled.setTextXY(7,3); 
 oled.putString("PLEASE :)");
 
    onlineMode();
 
 }
     digitalWrite(power,LOW);
  }

  




// -------------------------------------------------   Screen if run is paused -----------------------------------------

  void pauseFunc(){
    digitalWrite(power,LOW);
    oled.clearDisplay();             
  oled.drawBitmap(paused, 1024);   
    plusRead=digitalRead(buttonPlus);
  minusRead=digitalRead(buttonMinus);
  

while (minusRead==HIGH){
  ESP.wdtFeed();
  plusRead=digitalRead(buttonPlus);
  minusRead=digitalRead(buttonMinus);
  if (plusRead==LOW){
    digitalWrite(power,HIGH);
 gelRun();
 
  }
}
haveANice();
timer=0;
    }

 //--------------------------------------------   send " time left " to thingspeak.com -------------------------------------------

    void tps(){

      
  
 

  if (client.connect(server,80)) {  //   "184.106.153.149" or api.thingspeak.com
    String postStr = apiKey;
           postStr +="&field1=";
           postStr += String(timer-1);
          // postStr +="&field2=";
           //postStr += String(h);
           postStr += "\r\n\r\n";
 
     client.print("POST /update HTTP/1.1\n");
     client.print("Host: api.thingspeak.com\n");
     client.print("Connection: close\n");
     client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
     client.print("Content-Type: application/x-www-form-urlencoded\n");
     client.print("Content-Length: ");
     client.print(postStr.length());
     client.print("\n\n");
     client.print(postStr);
 
    
  }
  client.stop();
 
 
  delay(500);
  }
//-----------------------------------------------------  online mode function   -------------------------------------------

  void onlineMode(){
while(timer>0){
    tps();
    
    for(int k=0;k<60;k++){
    delay(900); 
    reedRead=digitalRead(reed);

  if (reedRead==HIGH){
     digitalWrite(power,LOW);
  oled.clearDisplay();             
  oled.drawBitmap(lid_open, 1024);   

    }
    
    while (reedRead==HIGH){
  delay(50);           // ESP.wdtFeed() is important in all cycles where you want only wait and polling. It feeds the software watchdog, if you do this not, in 3 secs resets the processor.
                              // Disable the soft wdt helps not, in this case starts the hardware watchdog immediately and resets in every 6 secs.... I've learned this through a bit sucks :/
  reedRead=digitalRead(reed);
  
   }
 digitalWrite(power,HIGH);
    
 oled.clearDisplay();
 oled.setTextXY(0,1); 
 oled.putString("DEVICE ONLINE");
 oled.setTextXY(3,2); 
 oled.putString("CHECK YOUR");
 oled.setTextXY(5,0); 
 oled.putString("CONNECTED PHONE");
 oled.setTextXY(7,3); 
 oled.putString("PLEASE :)");
 
    }  
                            
     timer--;

  //-----------------------------------------------      if the connection lost goes to offline mode :)   ------------------------------------

      if(WiFi.status()==WL_NO_SSID_AVAIL){       

    offlineMode();
 
   }

    

}
    

   
    }

 //----------------------------------------------------------- offline mode function with button sensing ------------------------------------------
void offlineMode(){
reedCheck();
   unsigned long currTime=0;
   unsigned long prevTime=0;
   int delayTime = 0;
   
 while(timer>0){

  
   
    for(int i=0;i<9;i++){

       if(timer<1){
      break;
      } 
         
  oled.clearDisplay();             
  oled.drawBitmap(powered, 1024);   
 
  ESP.wdtFeed();
  delayTime=0;
  prevTime=millis();
  while(delayTime<2000){
    reedCheck();
    currTime=millis();
    ESP.wdtFeed();
     plusRead=digitalRead(buttonPlus);
     minusRead=digitalRead(buttonMinus);
     if (plusRead==LOW){
      lightOn();
      }
      if (minusRead==LOW){
        delay(500);
       pauseFunc();
         
        }
        delayTime=currTime-prevTime;
      if(timer<1){
      break;   
    }
    }

    if(timer<1){
      break;
    }

 oled.clearDisplay(); 
 oled.setTextXY(0,4); 
 oled.putString("TIME LEFT :");
 oled.setTextXY(4,7); 
 oled.putNumber(timer);
 oled.setTextXY(7,5); 
 oled.putString("MINUTES");
 delayTime=0;
 prevTime=millis();
  while(delayTime<2000){
    ESP.wdtFeed();
    reedCheck();
    currTime=millis();
     plusRead=digitalRead(buttonPlus);
     minusRead=digitalRead(buttonMinus);
     if (plusRead==LOW){
      lightOn();
      }
      if (minusRead==LOW){
        delay(500);
       pauseFunc();
     
        }
        delayTime=currTime-prevTime;
         if(timer<1){
      break;
    }
  }
  if(timer<1){
      break;
  }
  oled.clearDisplay();             
  oled.drawBitmap(light_or_stop, 1024);   
  delayTime=0;
  prevTime=millis();
  while(delayTime<2000){
    reedCheck();
    currTime=millis();
    ESP.wdtFeed();
     plusRead=digitalRead(buttonPlus);
     minusRead=digitalRead(buttonMinus);
     if (plusRead==LOW  )
     {    lightOn();
      }
      if (minusRead==LOW){
        delay(500);
       pauseFunc();
    
        }
        delayTime=currTime-prevTime;
      if(timer<1){
      break;
    }
    }
    }
    timer--;
   
 }
    
}  

