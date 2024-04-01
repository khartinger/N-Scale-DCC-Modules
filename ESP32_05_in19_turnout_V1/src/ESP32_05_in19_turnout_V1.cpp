//_____ESP32_05_in19_turnout_V1.cpp______________khartinger_____
// This program for an ESP32 is used to test the control of a 
// model railroad turnout with limit switching.
//  The command to switch the turnout is given by a pushbutton
// connected to the ESP32.
// The turnout drive is switched via two pins of an I2C expander
// PCF8574 and a relay circuit (DIY circuit U5_W).
// Feedback is provided via two pins of a second PCF8574 I2C
// expander and is shown on a 1.54" OLED display.
// Hardware:
// 1. ESP32 D1 mini   
// 2. 1x DIY board I2C_3V3_5V   
// 3. 1x OLED display with SSD1309 controller (e.g. 1.54" or 
//    2.4" displays with 128x64 pixel resolution) on the 
//    DIY board I2C_3V3_5V   
// 4. 2x I²C expander boards PCF8574 with the (7-bit) addresses
//    0x20 and 0x21 on the DIY board I2C_3V3_5V   
// 5. a button on pin D6 (IO19) with pull-up resistor 
//    (e.g. 10 kOhm) to 3.3V (a connection for this is available
//    on the DCC_3V3 DIY board, for example)   
//--------------------------------------------------------------
// 6. a turnout with limit switch
// 7. a transformer with 16V alternating voltage (V+, V-)
// 8. the self-made U5_W boards for controlling the turnout 
//    with 5V
// Created by Karl Hartinger, April 01, 2024
// Changes:
// 2024-04-01 New
// Released into the public domain.
#include <Arduino.h>
#include "src/statemachine/D1_class_Statemachine.h"
#include "src/pcf8574/D1_class_PCF8574.h"
#include "src/screen154/D1_class_Screen154.h"

#define  DEBUG_05       true                // OR false
#define  VERSION_05     "2024-04-01 E32_ESP32_05_in19_turnout_V1"
#define  VERSION_05_1   "Version 2024-04-01"

//_______Global values and hardware_____________________________
const int pinButton=19;                     // 19=D6
int       buttonOld=-1;                     // old button value
int       iWSA=1;                           // turnout set value
int       iWRA=-1;                          // response value
int       iWRB=-1;                          // response value
int       iWRAOld=-1;                       // response value
int       iWRBOld=-1;                       // response value

//_______1.54" display data (SSD1309, 128x64 pixel, I2C)________
#define  SCREEN_TITLE        "05: in19_turnout"
#define  SCREEN_LINE_MAX     6
#define  SCREEN_LINE_LEN     21
Screen154 screen_;

//_______Hardware: IO expander PCF8574__________________________
PCF8574  pcf8574_out(0x20);            // 8 digital OUT
PCF8574  pcf8574_in (0x21);            // 8 digital IN
#define  BIT_WSA        0              // bit set WA
#define  BIT_WSB        1              // bit set WB
#define  BIT_WRA        0              // bit response WA
#define  BIT_WRB        1              // bit response WB

//_______State machine__________________________________________
#define STATE_MAX            36000     // 36000*100ms = 1 hour
#define STATE_DELAY            100     // state delay in ms
#define STATES_TURNOUT_ON       10     // 10*100ms=1s
int32_t stateOff=-1;                   // state turnoff turnout
Statemachine stm(STATE_MAX, STATE_DELAY); //1..36000

//_______read turnout response__________________________________
// uses  iWRA, iWRB, iWRAOld, iWRBOld
String readResponse() {
 String s1="";
 iWRA=pcf8574_in.getBit(BIT_WRA);
 iWRB=pcf8574_in.getBit(BIT_WRB);
 if(iWRA!=iWRAOld || iWRB!=iWRBOld) {
  if(iWRA==0 && iWRB==0) s1="0?";
  if(iWRA==0 && iWRB==1) s1="__";
  if(iWRA==1 && iWRB==0) s1="_/";
  if(iWRA==1 && iWRB==1) s1="1?";
 }
 iWRAOld=iWRA;
 iWRBOld=iWRB;
 return s1;
}

//_____SETUP____________________________________________________
void setup() {
 String s1="", s2="";
 //------Serial, just for debug---------------------------------
 if(DEBUG_05) {
  Serial.begin(115200);
  Serial.println("\nsetup(): --Start--");
 }
 //------hardware-----------------------------------------------
 //pinMode(BUILTIN_LED, OUTPUT);              // build in led
 pinMode(pinButton, INPUT);                 // switch turnout
 //------Init 1.54" display (SSD1309, 128x64 pixel)-------------
 screen_.begin();                           // start i2c
 //screen_.useCP437();                      // after screen_.begin();
 screen_.setFontText(u8g2_font_KH_cp437_6x8_mf); //
 screen_.useFontText();                     // write text
 screen_.setFontRefHeightText();            // (default)
 screen_.setFontPosTop();                   // font position
 screen_.screen15Clear(0, SCREEN_TITLE, 'c');
 screen_.screen15(SCREEN_LINE_MAX, VERSION_05_1);
 //------Init first 8-Bit I/O Expander PCF8574------------------
 s1="20";
 s2="Found";
 screen_.screen15(3,"Search PCF8474 0x"+s1);
 screen_.sendBuffer();
 while(!pcf8574_out.begin(false)) {  // I2C started
  if(DEBUG_05) {
   Serial.println("Error: "+pcf8574_out.getsStatus());
   Serial.print(" - Could not find PCF8574 at "+s1);
   Serial.println(": check wiring");
  }
  screen_.screen15(4,s1+" NOT found - Check wiring!");
  screen_.sendBuffer();
  delay(5000);                             // wait 5s
 }
 s2+=" 0x"+s1;                             // add address to found
 if(DEBUG_05) { Serial.println(s2); }
 screen_.screen15(4,s1+" found!");
 //------Init second 8-Bit I/O Expander PCF8574-----------------
 s1="21";
 screen_.screen15(3,"Search PCF8474 0x"+s1);
 screen_.sendBuffer();
 while(!pcf8574_in.begin(false)) {  // I2C started
  if(DEBUG_05) {
   Serial.println("Error: "+pcf8574_in.getsStatus());
   Serial.print(" - Could not find PCF8574 at "+s1);
   Serial.println(": check wiring");
  }
  screen_.screen15(4,s1+" NOT found - Check wiring!");
  screen_.sendBuffer();
  delay(5000);                             // wait 5s
 }
 s2+=" 0x"+s1;                             // add address to found
 if(DEBUG_05) { Serial.println(s2); }
 screen_.screen15(4,s2);
 if(DEBUG_05) Serial.println("setup(): --Finished--\n");
}

//_____LOOP_____________________________________________________
void loop() {
 //======(1) do at the beginning of the loop ...================
 int32_t state=stm.loopBegin();             // state begin
 String s1;
 String sSerial=String(state);
 //======(2) do, independent on the network, ...================
 //------read button--------------------------------------------
 int button_=digitalRead(pinButton);
 if(button_==0) {
  //.....button pressed.........................................
  if(button_!=buttonOld) {
   //....first time that button is pressed......................
   if(DEBUG_05) Serial.println("Button pressed!");
   iWSA=1-iWSA;                             // toggle value
   pcf8574_out.setBit(BIT_WSA, iWSA);
   pcf8574_out.setBit(BIT_WSB, 1-iWSA);
   stateOff = state + STATES_TURNOUT_ON;
  }
 }
 buttonOld=button_;
 //------read turnout response----------------------------------
 s1=readResponse();
 if(s1!="") {
  screen_.screen15(3,s1);
  screen_.sendBuffer();
  if(DEBUG_05) Serial.println("Turnout "+s1);
 }
 //------turnoff turnout voltage?-------------------------------
 if(state==stateOff)
 {
  pcf8574_out.setBit(BIT_WSA, 1);
  pcf8574_out.setBit(BIT_WSB, 1);
  stateOff=-1;
  if(DEBUG_05) Serial.println("Turnout off!");
 }

 //======(3) process mqtt actions===============================
 //======(4) do, depending on the network access, ...===========
 //======(5) do things after mqtt access========================
 //======(6) do at the end of the loop ...======================
 uint32_t ms=stm.loopEnd();                   // state end
 //------print serial data--------------------------------------
 if(DEBUG_05) {
  Serial.print(sSerial+" | "); Serial.print(ms); 
  if(ms>STATE_DELAY) Serial.println("ms-Too long!!");
  else Serial.println("ms");
 }
}