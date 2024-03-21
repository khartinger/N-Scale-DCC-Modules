//_____ESP32_02_I2C_test_pcf8574_oled154_V1______khartinger_____
// This program for an ESP32 D1 mini is used to test the 
// self-built board I2C_3V3_5V, which provides two connections 
// for I2C bus components.
// The connections differ in terms of the pin assignment and the
// supply voltage level.   
// The connection for OLED displays (1.54" or 2.4") has 3.3V as
// the supply voltage, the connection for PCF8574 expander 
// boards is operated with 5V, whereby the data lines for the
// processor are converted to 3.3V.    
// Hardware:
// 1. ESP32 D1 mini
// 2. OLED 1.54" with SSD1309 controller 128x64 pixel
// 3. PCF8574 board with (default) address 0x20
// 4. PCF8574 board with address 0x21
//    I2C connection: SCL=D1=GPIO5, SDA=D2=GPIO4, 3V3, GND
//
// Created by Karl Hartinger, March 21, 2024
// Changes:
// 2024-03-21 New
// Released into the public domain.

#include <Arduino.h>
#include "src/pcf8574/D1_class_PCF8574.h"
#include "src/screen154/D1_class_Screen154.h"

#define  DEBUG_02       true                // OR false
#define  VERSION_02     "2024-03-21 ESP32_02_I2C_test_pcf8574_oled154_V1"
#define  VERSION_02a   "Version 2024-03-21"

//_______Global values and hardware_____________________________

//_______1.54" display data (SSD1309, 128x64 pixel, I2C)________
#define  SCREEN_TITLE   "I2C Test"
#define  SCREEN_LINE_MAX 6
#define  SCREEN_LINE_LEN 21
//       buffer for lines for screen update
String   aScreenText[SCREEN_LINE_MAX];
//       sign per line: 1=normal display, -1=inverted
int      aScreenSign[SCREEN_LINE_MAX]={1,1,1,1,1,1};
Screen154 screen_;

//_______Hardware: IO expander PCF8574__________________________
#define  IOEX_NUM       2              // number of IO expander
PCF8574  pcf8574_0(0x20);              // 8 digital OIs
PCF8574  pcf8574_1(0x21);              // 8 digital IN
PCF8574 *pIOEx[IOEX_NUM]={&pcf8574_0, &pcf8574_1}; // IO expander

//_______Hardware: IO expander PCF8574__________________________
PCF8574  pcf8574(0x20);                // 8 digital i/0s

//_______Save line content, write it to display and show screen_
// line_: 0 ... SCREEN_LINE_MAX (6)
// uses: screen_, aScreenSign[], aScreenText[]
// function saves the text (in aScreenText[])
//    BUT DOES NOT change the sign of a line (invert text)
void showLine(int line_, String text_) {
 //------Save line for refeshScreen()---------------------------
 int lineAbs=line_;
 if(lineAbs<0) lineAbs=-lineAbs;
 if(lineAbs>=0 && lineAbs<=SCREEN_LINE_MAX) {
  if(lineAbs==0) aScreenText[lineAbs]=text_;
  else aScreenText[lineAbs-1]=text_;
  screen_.screen15(line_,"                     ");
  //-----Prepare line-------------------------------------------
  if(line_==0) {
   screen_.screen15Clear(0,text_,'c');       // title center, rect
  } else {
   screen_.screen15(line_,text_,'l');        // left aligned
  }
  //-----Show screen--------------------------------------------
  screen_.sendBuffer();                     // show screen
 }
}

//_______Refresh screen to avoid damage_________________________
// uses: screen_, aScreenSign[], aScreenText[]
void refreshScreen() {
 //------Clear screen-------------------------------------------
 screen_.screen15Clear(0,aScreenText[0],'c'); // centered title
 for(int i=1; i<SCREEN_LINE_MAX; i++) { 
  screen_.screen15(aScreenSign[i]*(i+1),aScreenText[i]);
 }
 //------Show screen--------------------------------------------
 screen_.sendBuffer();                      // show screen
}

//_____SETUP____________________________________________________
void setup() {
 String s1="", s2="", s3="";
 //------Serial, just for debug---------------------------------
 if(DEBUG_02) {
  Serial.begin(115200);
  Serial.println("\nsetup(): --Start--");
 }
 //------Init 1.54" display (SSD1309, 128x64 pixel)-------------
 screen_.begin();                           // start i2c
 //screen_.useCP437();                      // after screen_.begin();
 screen_.setFontText(u8g2_font_KH_cp437_6x8_mf); //
 screen_.useFontText();                     // write text
 screen_.setFontRefHeightText();            // (default)
 screen_.setFontPosTop();                   // font position
 showLine(0, SCREEN_TITLE);                 // show title
 showLine(SCREEN_LINE_MAX, VERSION_02a);   // show version
 delay(1000);

 //------Init all 8-Bit I/O Expander PCF8574--------------------
 s2="Found";
 bool bfirstComp=true;
 for(int i=0; i<IOEX_NUM; i++) {
  //aIOEx[i].setInvertOutput(true);
  s1=String((*pIOEx[i]).getAddress(), 16);
  s3="Search PCF8474 0x"+s1;
  s3=s3.substring(0,SCREEN_LINE_LEN);       // max. 21 character
  showLine(3,s3);
  while (!(*pIOEx[i]).begin(bfirstComp)) {  // I2C started
   if(DEBUG_02) {
    Serial.println("Error: "+(*pIOEx[i]).getsStatus());
    Serial.print(" - Could not find PCF8574 at 0x");
    Serial.print((*pIOEx[i]).getAddress(), 16);
    Serial.println(": check wiring");
   }
   showLine(4,s1+" NOT found - Check wiring!");
   delay(5000);                             // wait 5s
  }
  bfirstComp=false;
  s2+=" 0x"+s1;                             // add address to found
  (*pIOEx[i]).setByte(0xFF);
  showLine(5,s2);                           // show found addresses
  if(DEBUG_02) { Serial.println(s2); }
 }
 showLine(4, "");                           // clear "search"-line
 //------Finish setup-------------------------------------------
 if(DEBUG_02) Serial.println("setup(): --Finished--\n");
}

int32_t counter=0;

//_____LOOP_____________________________________________________
void loop() {
 if((counter++)%40==1) refreshScreen();     // every 4 secs
 delay(100);
}
