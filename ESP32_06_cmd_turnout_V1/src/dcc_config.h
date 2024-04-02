//_____dcc_config.h______________________________khartinger_____
// Configure file for ESP32 railroad DCC decoder
//
// Created by Karl Hartinger, April 02, 2024
// Changes:
// 2024-04-02 New
// Released into the public domain.

#ifndef DCC_CONFIG_H
 #define DCC_CONFIG_H
 #include <Arduino.h>                  // String, int32_t
 #include "src/pcf8574/D1_class_PCF8574.h"

//_______program version________________________________________
#define  VERSION_06     "2024-04-02 ESP32_06_cmd_turnout"
#define  VERSION_06_1   "Version 2024-04-02"

//_______Network and MQTT data__________________________________
#define  _USE_WIFI_     true
#define  _SSID_         "Raspi11"
#define  _PASS_         "12345678"
#define  _HOST_         "10.1.1.1"
#define  TOPIC_BASE     "turnout"
#define  TOPIC_GET      "?,help,version,ip,topicbase,eeprom"
#define  TOPIC_SET      "topicbase,eeprom0,byname,bydcc"
#define  TOPIC_SUB      ""
#define  TOPIC_PUB      ""

//_______1.54" display data (SSD1309, 128x64 pixel, I2C)________
#define  SCREEN_TITLE   "06_cmd_turnout"
#define  SCREEN_LINE_MAX 6
#define  SCREEN_LINE_LEN 21

//_______DCCex__________________________________________________
#define  DCC_OFFSET     4

//_______Hardware: IO expander PCF8574__________________________
#define  IOEX_NUM       2              // number of IO expander
PCF8574  pcf8574_out(0x20);            // 8 digital OUT
PCF8574  pcf8574_in (0x21);            // 8 digital IN
PCF8574 *pIOEx[IOEX_NUM]={&pcf8574_out, &pcf8574_in}; // IO expander

//_______Definitions for railroad components____________________
//.......values for every railroad component....................
// e.g. turnout, uncoupler, disconnectable track, ...
#define  NO_PIN         -1   // pin @ PCF8574 (0...7)
#define  RC_TYPE_TO     1    // turnout (Weiche)
#define  RC_TYPE_UC     2    // uncoupler (Entkuppler)
#define  RC_TYPE_DT     3    // disconnectable track (Fahrstrom)

//.......All properties of a railroad component.................
struct strRcomp {
  int    type;          // RC_TYPE_TO, RC_TYPE_UC, RC_TYPE_DT
  String name;          // short name like T1, U1, D1, W1, E1...
  int    dcc;           // dcc address of the component
  int    outPCF;        // aIOEx index of PCF8574 output device
  int    outBitA;       // bit PCF8574 for turnout stright (Gerade)
  int    outBitB;       // bit PCF8574 for turnout curved (Abzweig)
  int    inPCF;         // aIOEx index of PCF8574 input device
  int    inBitA;        // bit number at PCF8574 input stright=1
  int    inBitB;        // bit number at PCF8574 input curved=1
};

//_______Railroad commands______________________________________
// railway components:  type,name,dcc,
//                      pIOEx-out-index,outBitA,outBitB, 
//                      pIOEx-in-index inBitA inBitB
#define  RCOMP_1        RC_TYPE_TO,"T1",11, 0,0,1, 1,0,1
//.......Array of all railway components........................
#define  RCOMP_NUM      1
strRcomp aRcomp[RCOMP_NUM] = {{RCOMP_1}};
#endif