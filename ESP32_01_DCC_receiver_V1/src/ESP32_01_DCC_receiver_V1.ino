//_____ESP32_01_DCC_receiver_V1.cpp______________khartinger_____
// This program for an ESP32 D1 mini is used to test the 
// self-built DCC_3V3 board, which converts a DCC signal 
// into a 3.3Volt signal.
// If the program receives a DCC command (at D5 = IO18) or 
// if the button at D6 (IO19) is pressed, the built-in LED is 
// switched over and a counter is incremented.
// The event is also displayed on the serial interface.
// Hardware:
// 1. ESP32 D1 mini
// 2. Selfmade board DCC_3V3
// Created by Karl Hartinger, March 19, 2024
// Changes:
// 2024-03-19 New
// Released into the public domain.

#include <Arduino.h>                        // String, int32_t
#include <DccAccessoryDecoder.h>
#define  DEBUG_01       true                // OR false

//_______Global values and hardware_____________________________
#define BUTTON_PIN      19                  // IO19=D6
int buttonValue=-1;                         // button value
int buttonOld=-2;                           // old button value
int ledValue=0;                             // led value

//_______dcc access_____________________________________________
#define  DCC_OFFSET     4                   // 4 for multiMaus
#define  DCC_PIN        18                  // IO18=D5
unsigned int dccAddress=0;                  // dcc address
int dccValue=-1;                            // dcc value (0|1)
int32_t counter=1;                          // event counter

//_______DCC request____________________________________________
void onAccessoryPacket(unsigned int linearDecoderAddress, bool enabled) {
 ledValue=1-ledValue;
 digitalWrite(BUILTIN_LED, ledValue);
 dccAddress=(int)linearDecoderAddress + DCC_OFFSET;
 dccValue=enabled ? 1 : 0;
 if(DEBUG_01) {
  Serial.print("#");
  Serial.print(counter++);
  Serial.print(" | onAccessoryPacket(): DCC Address ");
  Serial.print(dccAddress);
  Serial.print(", Value ");
  Serial.println(dccValue);
 }
}

//_____SETUP____________________________________________________
void setup() {
 //------Serial, just for debug---------------------------------
 if(DEBUG_01) {
  Serial.begin(115200);
  Serial.println("\nsetup(): --Start--");
 }
 //------hardware-----------------------------------------------
 pinMode(BUILTIN_LED, OUTPUT);              // led
 pinMode(BUTTON_PIN, INPUT);                // button
 
 //------DCC: register pin and callback routine-----------------
 DccAccessoryDecoder.begin(DCC_PIN, onAccessoryPacket);
  if(DEBUG_01) Serial.println("setup(): --Finished--\n");
}

//_____LOOP_____________________________________________________
void loop() {
 //------button handling----------------------------------------
 buttonValue=digitalRead(BUTTON_PIN);
 if(buttonValue==0) {
  //.....button pressed (0).....................................
  if(buttonOld==1) {
   if(DEBUG_01) {
    Serial.print("#");
    Serial.print(counter++);
    Serial.println(" | Button pressed");
   }
   ledValue=1-ledValue;
   digitalWrite(BUILTIN_LED, ledValue);
  }
 }
 buttonOld=buttonValue;
 //------look for dcc event-------------------------------------
 DccAccessoryDecoder.loop();
}