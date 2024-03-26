//_____ESP32_04_statemachine_V1.cpp______________khartinger_____
// This program for a D1mini (or ESP32) shows
// .....
// Blinklicht 1s
// Hardware:
// 1. WeMos D1 mini (OR ESP32 D1 mini)
// Created by Karl Hartinger, March 26, 2024
// Changes:
// 2024-03-26 New
// Released into the public domain.
//#define D1MINI          1              // ESP8266 D1mini +pro
//#define ESP32D1         2              // ESP32 D1mini

#include <Arduino.h>                  // String, int32_t
#include "src/statemachine/D1_class_Statemachine.h"

#define  DEBUG_04       true                // OR false
#define  VERSION_04     "Version 2024-03-26 ESP32_04_statemachine_V1.cpp"

//_______Global values and hardware_____________________________
int iLedValue=1;             // ESP32: 1=ON

//_______State machine__________________________________________
#define STATE_MAX            36000     // 36000*100ms = 1 hour
#define STATE_DELAY            100     // state delay in ms
#define STATES_BLINK            10     // 10*100ms =1s
Statemachine stm(STATE_MAX, STATE_DELAY); //1..36000

//_____SETUP____________________________________________________
void setup() {
 //------Serial, just for debug---------------------------------
 if(DEBUG_04) {
  Serial.begin(115200);
  Serial.println("\nsetup(): --Start--");
 }
 //------hardware-----------------------------------------------
 pinMode(BUILTIN_LED, OUTPUT);              // build in led
 digitalWrite(BUILTIN_LED, iLedValue);      // set LED
 //------use WiFi and MQTT?-------------------------------------
 if(DEBUG_04) Serial.println("setup(): --Finished--\n");
}

//_____LOOP_____________________________________________________
void loop() {
 //======(1) make at the beginning of the loop ...==============
 int state=stm.loopBegin();                 // state begin
 String s1;
 String sSerial=String(state);

 //======(2) do, independent on the network, ...================
 //------flashing LED-------------------------------------------
 if(state % STATES_BLINK == 0) {
  iLedValue = 1-iLedValue;                   // invert LED
  digitalWrite(BUILTIN_LED, iLedValue);      // set LED
 }
 sSerial+=" | iLedValue ="+String(iLedValue);
 
 //======(3) process mqtt actions===============================
 //======(4) do, depending on the network access, ...===========
 //======(5) do things after mqtt access========================
 //======(6) do at the end of the loop ...======================
 uint32_t ms=stm.loopEnd();                   // state end
 //------print serial data--------------------------------------
 if(DEBUG_04) {
  Serial.print(sSerial+" | "); Serial.print(ms); 
  if(ms>STATE_DELAY) Serial.println("ms-Too long!!");
  else Serial.println("ms");
 }
}