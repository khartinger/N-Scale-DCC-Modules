//_____ESP32_06_cmd_turnout_V1___________________khartinger_____
// This program for an ESP32 is used to test the control of a
// model railroad turnout with limit switching via DCC or MQTT.
// The turnout drive is switched via two pins of an I2C expander
// PCF8574 and a relay circuit (self-made circuit U5_W).
// The feedback signal is sent via two pins of a second I2C 
// expander PCF8574 and is shown on a 1.54" OLED display.
// A button on pin D6 (IO19) can be used to cancel this step
// during the search for the WLAN and work without MQTT 
// (e.g. if the WLAN is not accessible).
// If you press the button for one second while the program is
// running, a reset is triggered. This can be used, for example,
// to activate the WLAN while the program is running.
// All project-specific data, such as WLAN access, MQTT commands
// and hardware properties, are stored in a configuration file
// `dcc_config.h`.
//
// Hardware:
// 1. ESP32 D1 mini
// 2. 1x DIY board I2C_3V3_5V
// 3. 1x OLED display with SSD1309 controller (e.g. 1.54" or
//    2.4" displays with 128x64 pixel resolution) on the 
//    DIY board I2C_3V3_5V
// 4. 2x I²C expander boards PCF8574 with the (7-bit) addresses
//    0x20 and 0x21 on the DIY board I2C_3V3_5V
// 5. 1x DIY board DCC_3V3 for connecting the DCC signal
// 6. a button on pin D6 (IO19) with pull-up resistor 
//    (e.g. 10 kOhm) after 3.3V (a connection for this is 
//    available on the DCC_3V3 DIY board, for example)
//--------------------------------------------------------------
// 7. a turnout with limit switch
// 8. a transformer with 16V alternating voltage (V+, V-)
// 9. the self-made U5_W boards for controlling the turnout 
//    with 5V
// 10. a DCC source for sending turnout commands (e.g. MultiMAUS
//     with digital amplifier 10764 and power supply 10850)
//
// Class SimpleMqtt extends class PubSubClient for easy use.
// All commands of the PubSubClient class can still be used.
// Note: When PubSubClient lib is installed,
//       delete PubSubClient files in directory src/simplemqtt
// Important: Example needs a MQTT-broker!
// Created by Karl Hartinger, April 02, 2024
// Changes:
// 2024-04-02 New
// Released into the public domain.
//#define D1MINI          1              // ESP8266 D1mini +pro
#define ESP32D1         2              // ESP32 D1mini

// #include <Arduino.h>
// #include "src/pcf8574/D1_class_PCF8574.h"
#include <DccAccessoryDecoder.h>
#include "dcc_config.h"
#include "src/statemachine/D1_class_Statemachine.h"
#include "src/simplemqtt/D1_class_SimpleMqtt.h"
#include "src/screen154/D1_class_Screen154.h"

#define  DEBUG_06       true                // OR false
#define  LANGUAGE_06    'e'                 // 'd' or 'e'

//_______declare method(s)______________________________________
String setRcompCmd(int iRcomp, int iCmdValue, String sReturn);

//_______Global values and hardware_____________________________
#define  PIN_DCC        18                  // 18=D5
#define  PIN_BUTTON     19                  // 19=D6

//_______Global values for connection state_____________________
bool     bUseWiFi=false;
int      iConn=0;                          // WiFi unknown
int      iConnOld=0;                       // WiFi unknown
const    String sConn[]={"-unknown--", "connecting", 
         "-No WiFi--", "-No MQTT--", "WiFi OK   ", "MQTT OK   ",
        "Unused WiFi"};

//_______Global values for updating the screen__________________
//       buffer for lines for screen update
String   aScreenText[SCREEN_LINE_MAX];
//       sign per line: 1=normal display, -1=inverted
int      aScreenSign[SCREEN_LINE_MAX]={1,1,1,-1,1,1};
Screen154 screen_;

//_______Railroad commands______________________________________
//.......PCF8574 lines B A to control turnout (active low!)
#define  CMD_NONE       -1   // no command
#define  CMD_BITA_ON    1    // set bit A=0 (inverted)
#define  CMD_BITA_OFF   2    // set bit A=1 (inverted)
#define  CMD_BIT2_ON    3    // bits BA = 00
#define  CMD_BIT2_A     4    // bits BA = 10
#define  CMD_BIT2_B     5    // bits BA = 01
#define  CMD_BIT2_OFF   6    // bits BA = 11

//.......All properties of a railroad command...................
// command: 0=out, 1=stright, 2=curved, 3=undefined (switching)
struct strRcmd {
  int     iCmd;         // command - what to do (now)
  int     inValue;      // current input value
  int32_t stateToDo;    // in which state should command be executed?
  int32_t stateOffset;  // additional state offset value
  int     iCmdOffset;   // command - what to do @ offset state
};
//.......Default values for all railroad commands...............
#define RCMD_NONE       CMD_NONE,-1,STATE_NONE,STATE_NONE,CMD_NONE
//.......Array of all railway commands..........................
// As there should only ever be one active command for a 
// component, aRcomp[] and aRcmd[] have the same size and the 
// same index for a command.
strRcmd aRcmd[RCOMP_NUM];

//_______State machine__________________________________________
#define STATE_MAX           180000     // 180000*20ms = 1 hour
#define STATE_DELAY             20     // state delay in ms
#define STATES_TURNOUT_ON       25     // 25*20ms=0,5s
#define STATES_UNCOUPLER_ON     75     // 75*20ms=10,5s
#define STATES_SCREEN_REFRESH  501     // 501*20ms=10,1s
#define STATES_BLINK            25     // 25*20ms =0,5s
#define STATES_BEFORE_RESET     50     // 25*20ms =1s
Statemachine stm(STATE_MAX, STATE_DELAY); //1..36000

//_______dcc access_____________________________________________
unsigned int dccAddress=0;             // dcc address
int dccValue=-1;                       // dcc input value

//_______DCC request____________________________________________
void onAccessoryPacket(unsigned int linearDecoderAddress, bool enabled) {
 digitalWrite(BUILTIN_LED, enabled ? 1 : 0);
 dccAddress=(int)linearDecoderAddress + DCC_OFFSET;
 dccValue=enabled ? 1 : 0;
 //------is it a DCC address for this module?-------------------
 for(int i=0; i<RCOMP_NUM; i++) {
  if(aRcomp[i].dcc==dccAddress) {
   //----railroad component found-------------------------------
   setRcompCmd(i, dccValue, "");
  }
 }
 if(DEBUG_06) {
  Serial.print("***Change in Accessory: ");
  Serial.print(dccAddress);
  Serial.print(" -> ");
  Serial.print(dccValue);
  Serial.println("***");
 }
}

#if _USE_WIFI_ == true
 //_______MQTT communication_____________________________________
 //SimpleMqtt client("..ssid..", "..password..","mqttservername");
 //SimpleMqtt client("Raspi11", "12345678","10.1.1.1", TOPIC_BASE);
 SimpleMqtt client(String(_SSID_), String(_PASS_),
                   String(_HOST_), String(TOPIC_BASE));

//_______MQTT: inspect all subscribed incoming messages_________
void callback(char* topic, byte* payload, unsigned int length)
{
 client.callback_(topic, payload, length);  // must be called!
}

//_______Answer get requests____________________________________
// sPayload: payload to message from TOPIC_GET
// auto answer: for help (+), version, ip (can be overwritten)
// return: ret answer payload for get request
String simpleGet(String sPayload)
{
 String p1=String("");                      // help string
 sPayload.toLowerCase();                    // for easy compare
 //-------------------------------------------------------------
  if(sPayload=="help" || sPayload=="?") {
  p1="+MQTT: ../set/w1 -m 1|g|G OR -m 0|A|a|B|b\r\n";
  p1+="      ../get -m w1\r\n";
  return p1;
 }
 //-------------------------------------------------------------
  if(sPayload=="version") {
  p1="{\"version\":\""; p1+= String(VERSION_06); p1+="\"}";
  return p1;
 }
 //-------------------------------------------------------------
 if(sPayload=="ip") {
  p1="{\"ip\":\""; p1+= client.getsLocalIP(); p1+="\"}";
  return p1;
 }
 //-------------------------------------------------------------
 if(sPayload=="topicbase") {
  p1="{\"topicbase\":\""; p1+=client.getsTopicBase(); p1+="\"}";
  return p1;
 }
 //-------------------------------------------------------------
 if(sPayload=="eeprom") {
  int iResult1, iResult2;
  String s1=client.eepromReadTopicBase(iResult1);
  String s2=client.eepromReadMyData(iResult2);
  p1="{\"topicbase\":\"";
  if(iResult1>=0) p1+=s1;
  else
  {
   if(iResult1==-2) p1+="(no topic base)";
   else p1+=String("Error_#")+String(iResult1);
  }
  p1+="\",\"mydata\":\"";
  if(s2=="") p1+="(no data)";
  else p1+=s2;
  p1+="\"}";
  return p1;
 }
 //-------------------------------------------------------------
 //------is it a get command for a railway component?-----------
 for(int i=0; i<RCOMP_NUM; i++) {
  String s1=String(aRcomp[i].name);
  s1.toLowerCase();
  if(sPayload==s1 || sPayload==String(aRcomp[i].dcc)) {
   p1="{\""+aRcomp[i].name+"\":\"";
   if(aRcomp[i].type==RC_TYPE_TO) {
    switch(aRcmd[i].inValue) {
     case 0: p1+="undefined"; break;
     case 1: p1+="Gerade"; break;
     case 2: p1+="Abzweig"; break;
     default: p1+="unknown"; break;
    }
   } else {
    p1+=String(aRcmd[i].inValue);
   }
   p1+="\"}";
   return p1;
  }
 }

 //-------------------------------------------------------------
 return String("");                         // wrong Get command
}

//_______Execute set requests___________________________________
// sTopic from TOPIC_SET, sPayload: payload to topic
// return: ret answer payload for set command
String simpleSet(String sTopic, String sPayload)
{
 String p1=String("");                      // help string
 sTopic.toLowerCase();
 sPayload.toLowerCase();                    // for easy compare
 //-------------------------------------------------------------
 if(sTopic=="topicbase") {                  // new topic base?
  client.changeTopicBase(sPayload);         // change base
  p1="{\"topicbase\":\"";                   // start json
  p1+=client.getsTopicBase();               // read new base
  p1+="\"}";                                // end json
  return p1;                                // return new base
 }
 //-------------------------------------------------------------
 if(sTopic=="eeprom0") {                    // erase eeprom?
  if(sPayload=="?") {
   p1="{\"erase\":\"";                      // start json
   p1+="1=topicBase|2=myData|3=all\"}";     // end json
   return p1;                               // return erase info
  }
  p1="";
  if(sPayload=="2" || sPayload=="3" || sPayload=="all")
  {
   p1+="my data ";
   if(!client.eepromEraseMyData()) p1+="NOT ";
   p1+="deleted";                           // return answer 1
  }
  if(sPayload=="1" || sPayload=="3" || sPayload=="all")
  {
   if(p1!="") p1+=", ";
   p1+="topicbase ";
   if(!client.eepromEraseTopicBase()) p1+="NOT ";
   p1+="deleted";                           // return answer 2
  }
  p1="{\"eeprom\":\""+p1+"\"}";             // make json
  return p1;
 }
 //-------------------------------------------------------------

 //------is it a set command for a railway component?-----------
 for(int i=0; i<RCOMP_NUM; i++)
 {
  String s1=String(aRcomp[i].name);
  s1.toLowerCase();
  if(sTopic==String(aRcomp[i].dcc) || sTopic==s1) {
   int iCmdValue=-1;
   if(aRcomp[i].type==RC_TYPE_TO) {
    if(sPayload=="0" ||  sPayload=="a" || sPayload=="b") iCmdValue=0;
    if(sPayload=="1" || sPayload=="g") iCmdValue=1;
   } else {
    if(sPayload=="0") iCmdValue=0;
    if(sPayload=="1") iCmdValue=1;
   }
   return(setRcompCmd(i, iCmdValue, sPayload));
  } // END OF if: set command for a railway component
 } // END OF for-loop
 //-------------------------------------------------------------
 return String("");                         // wrong set command
}

//_______Execute sub requests___________________________________
// sTopic from TOPIC_SUB, sPayload: payload to topic
// return: no automatic answer
void simpleSub(String sTopic, String sPayload)
{
 //-------------------------------------------------------------
}
#endif

//_______prepare a set command for a railway component__________
// uses: aRcomp
// return: answer string e.g. for WiFi answer
String setRcompCmd(int iRcomp, int iCmdValue, String sReturn) {
 if(aRcomp[iRcomp].type==RC_TYPE_TO) {
  //...it is a turnout command (2 bits, 2cmds)................
  if(iCmdValue==0) {
   aRcmd[iRcomp].stateToDo=STATE_NOW;
   aRcmd[iRcomp].iCmd=CMD_BIT2_A;             // turnout curved
   aRcmd[iRcomp].stateOffset=STATES_TURNOUT_ON;
   aRcmd[iRcomp].iCmdOffset=CMD_BIT2_OFF;     // turnout off
   return sReturn+String(" received");
  } else {
  //. .command turnout stright?. . . . . . . . . . . . . . . .
   if(iCmdValue==1) { 
    aRcmd[iRcomp].stateToDo=STATE_NOW;
    aRcmd[iRcomp].iCmd=CMD_BIT2_B;            // turnout stright
    aRcmd[iRcomp].stateOffset=STATES_TURNOUT_ON;
    aRcmd[iRcomp].iCmdOffset=CMD_BIT2_OFF;    // turnout off
    return sReturn+String(" received");
   }
  }
 } // END OF it is a turnout command (2 bits, 2cmds)............
 if(aRcomp[iRcomp].type==RC_TYPE_UC) {
  //...it is a uncoupler command (1 bit, 2cmds).................
  if(iCmdValue==0) {                        // turn current off
   aRcmd[iRcomp].stateToDo=STATE_NOW;       // now...
   aRcmd[iRcomp].iCmd=CMD_BITA_OFF;         // turn current off
   aRcmd[iRcomp].stateOffset=STATE_NONE;    // no state to do
   aRcmd[iRcomp].iCmdOffset=CMD_NONE;       // nothing to do
   return sReturn+String(" received");
  }
  if(iCmdValue==1) {
   aRcmd[iRcomp].stateToDo=STATE_NOW;       // now...
   aRcmd[iRcomp].iCmd=CMD_BITA_ON;          // turn current on
   aRcmd[iRcomp].stateOffset=STATES_UNCOUPLER_ON; // after some time
   aRcmd[iRcomp].iCmdOffset=CMD_BITA_OFF;   // turn current off
   return sReturn+String(" received");
  }
 } // END OF it is a uncoupler command (1 bit, 2cmds)...........

 if(aRcomp[iRcomp].type==RC_TYPE_DT) {
  //...it is a disconn track command (1 bit, 1cmd)............
  if(iCmdValue==0) {                        // turn current off
   aRcmd[iRcomp].stateToDo=STATE_NOW;       // now...
   aRcmd[iRcomp].iCmd=CMD_BITA_OFF;         // turn current off
   aRcmd[iRcomp].stateOffset=STATE_NONE;    // no state to do
   aRcmd[iRcomp].iCmdOffset=CMD_NONE;       // nothing to do
   return sReturn+String(" received");
  }
  if(iCmdValue==1) {
   aRcmd[iRcomp].stateToDo=STATE_NOW;       // now...
   aRcmd[iRcomp].iCmd=CMD_BITA_ON;          // turn current on
   aRcmd[iRcomp].stateOffset=STATE_NONE;    // no state to do
   aRcmd[iRcomp].iCmdOffset=CMD_NONE;       // nothing to do
   return sReturn+String(" received");
  }
 } // END OF it is a disconn track command (1 bit, 1cmd)......
 return sReturn+String(" - Error");
}

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

//_______Fill the screen lines with current content_____________
// uses: aScreenText[], aRcomp[]
void prepareScreenLine4to6(bool updateGroupNumber) {
 static int iRcompGroup=0;                  // number of group of 5
 String s1="";                              // help string
 aScreenText[3]="";                         // clear line 4
 aScreenText[4]="";                         // clear line 5
 aScreenText[5]="";                         // clear line 6
 int iRcStart=5*iRcompGroup;                // RComp start index
 int imax=RCOMP_NUM-iRcStart;               // last element to show
 for(int i=0; i<imax; i++) { // for max. 5 group elements
  int iRc=iRcStart+i;
  //-----railway component name max. 3 chars + blank = 4 chars--
  aScreenText[3]+=(aRcomp[iRc].name.substring(0,3)+"    ").substring(0,4);
  //-----generate symbol or value (line 5 of screen)------------
  if(aRcomp[iRc].type==RC_TYPE_TO) { // turnout
   switch(aRcmd[iRc].inValue) {
    case 0:  aScreenText[4]+="0?  "; break; // BA=00
    case 1:  aScreenText[4]+="_/  "; break; // BA=01 (curved)
    case 2:  aScreenText[4]+="__  "; break; // BA=10 (stright)
    case 3:  aScreenText[4]+="1?  "; break; // BA=11
    default: aScreenText[4]+="??  "; break; // ?? impossible
   }
  } else {
   if(aRcomp[iRc].type==RC_TYPE_DT) { // discon track (fahrstrom)
    aRcmd[iRc].inValue ? aScreenText[4]+="On  " : aScreenText[4]+="Off ";
   } else {
    if(aRcomp[iRc].type==RC_TYPE_UC) {
    aRcmd[iRc].inValue ? aScreenText[4]+="On  " : aScreenText[4]+="Off ";
    } else {
     aScreenText[4]+=(" "+String(aRcmd[iRc].inValue)+"   ").substring(0,4);
    }
   }
  }
  //-----dcc number of railway element--------------------------
  aScreenText[5]+=(String(aRcomp[iRc].dcc)+"    ").substring(0,4);
 } // END OF for 5 group elements
 //------Should the group number be changed?--------------------
 if(updateGroupNumber) {
  iRcompGroup++;
  if(5*iRcompGroup>RCOMP_NUM) iRcompGroup=0;
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

//_______Update input values____________________________________
// uses: aRcomp[]
// return: true = at least one input value has changed
bool updateInputValues() {
 bool bReturn=false;
 //------update input values------------------------------------
 for(int i=0; i<RCOMP_NUM; i++) {
  //.....read input values......................................
  int iTemp=0;                              // help value
  int iIOEx=aRcomp[i].inPCF;
  int iBit=aRcomp[i].inBitA;
  if(iBit!=NO_PIN) iTemp+=(*pIOEx[iIOEx]).getBit(iBit);
  iBit=aRcomp[i].inBitB;
  if(iBit!=NO_PIN) iTemp+=2*(*pIOEx[iIOEx]).getBit(iBit);
  //.....save input value.......................................
  if(aRcmd[i].inValue!=iTemp) {
   aRcmd[i].inValue=iTemp;        // save input value
   bReturn=true;
  }
 } // END OF update input values--------------------------------
 return bReturn;
}

//_______act on a command, Hardware_____________________________
// called by actOnCmd()
// uses: aIOEx[] (PCF8574)
// return: command string (for serial output)
//         or return "" if there was nothing to do
String actOnCmdHardware(int iCmd_, int iOutPCF_, 
                        int outBitA_, int outBitB_) {
 String sSerial_="";
 switch(iCmd_){
  case CMD_NONE:                            // No command
   sSerial_="cmd: No comnmand";
   break;
  case CMD_BITA_ON:                         // A=0 (active low)
   if(outBitA_!=NO_PIN) {
    (*pIOEx[iOutPCF_]).setBit(outBitA_, 0); // OK
    sSerial_="cmd: Pin A 0V";
   } else sSerial_="No pin A";
   break;
  case CMD_BITA_OFF:                        // A=1
   if(outBitA_!=NO_PIN) {
    (*pIOEx[iOutPCF_]).setBit(outBitA_, 1);
    sSerial_="cmd: Pin A 5V";
   } else sSerial_="No pin A";
   break;
  case CMD_BIT2_ON:                         // BA=00 (active low)
   if(outBitA_!=NO_PIN) {
    (*pIOEx[iOutPCF_]).setBit(outBitA_, 0);
    sSerial_="cmd: Pin A 0V, ";
   } else sSerial_="No pin A, ";
   if(outBitB_!=NO_PIN) {
    (*pIOEx[iOutPCF_]).setBit(outBitB_, 0);
    sSerial_+="Pin B 0V";
   } else sSerial_+="No pin B";
   break;
  case CMD_BIT2_A:                         // BA=10 (active low)
   if(outBitA_!=NO_PIN) {
    (*pIOEx[iOutPCF_]).setBit(outBitA_, 0);
    sSerial_="cmd: Pin A 0V, ";
   } else sSerial_="No pin A, ";
   if(outBitB_!=NO_PIN) {
    (*pIOEx[iOutPCF_]).setBit(outBitB_, 1);
    sSerial_+="Pin B 5V";
   } else sSerial_+="No pin B";
   break;
  case CMD_BIT2_B:                         // BA=01 (active low)
   if(outBitA_!=NO_PIN) {
    (*pIOEx[iOutPCF_]).setBit(outBitA_, 1);
    sSerial_="cmd: Pin A 5V, ";
   } else sSerial_="No pin A, ";
   if(outBitB_!=NO_PIN) {
    (*pIOEx[iOutPCF_]).setBit(outBitB_, 0);
    sSerial_+="Pin B 0V";
   } else sSerial_+="No pin B";
   break;
  case CMD_BIT2_OFF:                        // BA=11 (active low)
   if(outBitA_!=NO_PIN) {
    (*pIOEx[iOutPCF_]).setBit(outBitA_, 1);
    sSerial_="cmd: Pin A 5V, ";
   } else sSerial_="No pin A, ";
   if(outBitB_!=NO_PIN) {
    (*pIOEx[iOutPCF_]).setBit(outBitB_, 1);
    sSerial_+="Pin B 5V";
   } else sSerial_+="No pin B";
   break;
  default:
   sSerial_+="cmd: unknown "+String(iCmd_);
   break;
 }
 if(outBitA_==3) sSerial_+="***PinA=3***";
 return sSerial_;
}


//_______act on a command_______________________________________
// uses: aRcomp[] (railway components)
//       actOnCmdHardware() (aIOEx[] = PCF8574)
// return: command string (for serial output)
//         or return "" if there was nothing to do
String actOnCmd(int32_t state) {
 String sSerial_="";
 for(int i=0; i<RCOMP_NUM; i++) { // for all railway components
  //.....is this a 1st state to do something?...................
  if(state==aRcmd[i].stateToDo) {
   int iCmd_=aRcmd[i].iCmd;
   int iOutPCF_=aRcomp[i].outPCF; 
   int outBitA_=aRcomp[i].outBitA;
   int outBitB_=aRcomp[i].outBitB;
   sSerial_=actOnCmdHardware(iCmd_, iOutPCF_, outBitA_, outBitB_);
   aRcmd[i].stateToDo=STATE_NONE;
   aRcmd[i].iCmd=CMD_NONE;
  }
  //.....is this a 2nd state to do something?...................
  if(state==aRcmd[i].stateOffset) {
   int iCmd_=aRcmd[i].iCmdOffset;
   int iOutPCF_=aRcomp[i].outPCF; 
   int outBitA_=aRcomp[i].outBitA;
   int outBitB_=aRcomp[i].outBitB;
   sSerial_=actOnCmdHardware(iCmd_, iOutPCF_, outBitA_, outBitB_);
   aRcmd[i].stateOffset=STATE_NONE;
   aRcmd[i].iCmdOffset=CMD_NONE;
  }
 }
 return sSerial_;
}

//_____SETUP____________________________________________________
void setup() {
 bool bRet;
 int button_;
 String s1="", s2="", s3="";
 //------Serial, just for debug---------------------------------
 if(DEBUG_06) {
  Serial.begin(115200);
  Serial.println("\nsetup(): --Start--");
 }
 //------init railway commands----------------------------------
 for(int i=0; i<RCOMP_NUM; i++) {
  aRcmd[i]={RCMD_NONE};
 }
 //------hardware-----------------------------------------------
 pinMode(BUILTIN_LED, OUTPUT);              // build in led
 pinMode(PIN_BUTTON, INPUT);                 // button next display
 //------use WiFi and MQTT?-------------------------------------
 bUseWiFi=(_USE_WIFI_ ? true : false);
 //------Init 1.54" display (SSD1309, 128x64 pixel)-------------
 screen_.begin();                           // start i2c
 //screen_.useCP437();                      // after screen_.begin();
 screen_.setFontText(u8g2_font_KH_cp437_6x8_mf); //
 screen_.useFontText();                     // write text
 screen_.setFontRefHeightText();            // (default)
 screen_.setFontPosTop();                   // font position
 showLine(0, SCREEN_TITLE);                 // show title
 showLine(SCREEN_LINE_MAX, VERSION_06_1);   // show version
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
   if(DEBUG_06) {
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
  if(DEBUG_06) { Serial.println(s2); }
 }
 showLine(4, "");                           // clear "search"-line

 //------Setup WiFi/MQTT client---------------------------------
#if _USE_WIFI_ == true
 if(bUseWiFi) {
  client.setLanguage(LANGUAGE_06);           //e=english,d=german
  client.setCallback(callback);              // mqtt receiver
  client.setTopicBaseDefault(TOPIC_BASE);    // topic base
  client.setWiFiWaitingTime(1000);           // set a short time (1s)
  client.setWiFiConnectingCounter(19);       // try 10s connecting
  s1="";
  for(int i=0; i<RCOMP_NUM; i++) {
   s1+=","+aRcomp[i].name;
   s1+=","+String(aRcomp[i].dcc);
  }
  client.setTopics(String(TOPIC_GET)+s1,String(TOPIC_SET)+s1,TOPIC_SUB,TOPIC_PUB);
  //client.setRetainedIndex("get",3,true);
  client.begin();                            // setup objects
  //------Show connecting procedure-----------------------------
  s1="WiFi "+String(_SSID_)+" connecting...";
  s1=s1.substring(0,SCREEN_LINE_LEN);        // max. 21 character
  screen_.screen15(2,s1);                    // line 2: begin connect
  screen_.screen15(4,"Button: skip WiFi -->");   // line4: 
  if(DEBUG_06) Serial.print("setup(): "+s1+String("\n"));
  client.connectingWiFiBegin();              // begin connecting
  int iUseWiFi=30;
  //.....waiting for WiFi connection............................
  do {
   if(digitalRead(PIN_BUTTON)==0) { iUseWiFi=0; break; }
   bRet=client.connectingWiFi();             // try to connect
   screen_.screen15Dot(3);                   // line 3: waiting dot
   iUseWiFi--;
  } while(!bRet && iUseWiFi>0);
  //.....END OF waiting for WiFi connection.....................
  if(iUseWiFi>0) {
   //----WiFi ok (no timeout)-----------------------------------
   iConn=4;                                  // WiFi OK
   bUseWiFi=true;                            // use WiFi 
   s2=client.getsTopicBase();
   if(client.isMQTTConnected() || client.isMQTTConnectedNew()) {
    iConn=5;                                 // MQTT OK
    //----WiFi and MQTT OK: publish start info-------------------
    client.publish_P("info/start/setup",s2.c_str(),false);
    //client.bAllowMQTTStartInfo(false);     //NO mqtt (re)start info
    if(DEBUG_06) Serial.println(s2);
   }
  } else {
   //----WiFi timeout-------------------------------------------
   iConn=2;                                  // NO WiFi
   bUseWiFi=false;                           // don´t use WiFi
   s2="*No control via MQTT*";
   s2=s2.substring(0,SCREEN_LINE_LEN);       // max. 21 character
  }
  s1="WiFi "+ sConn[iConn]+ " " + String(_SSID_);
  showLine(2, s1);
  showLine(3, s2);
 }
#else
 //------Dont use WiFi anyway-----------------------------------
 iConn=2;                                  // NO WiFi
 s1="==No control via MQTT=";
 s1=s1.substring(0,SCREEN_LINE_LEN);       // max. 21 character
 showLine(2, s1);
 showLine(3, "");
#endif

 //------DCC: register pin and callback routine-----------------
 DccAccessoryDecoder.begin(PIN_DCC, onAccessoryPacket);
 //------Finish setup-------------------------------------------
 bool bTemp=updateInputValues();
 if(DEBUG_06) Serial.println("setup(): --Finished--\n");
}

int toggle=0;
int32_t reset_countdown=-1;

//_____LOOP_____________________________________________________
void loop() {
 //======(1) make at the beginning of the loop ...==============
 int state=stm.loopBegin();                 // state begin
 String s1;
 String sSerial=String(state);

 //======(2) do, independent on the network, ...================
 //------refresh screen-----------------------------------------
 if(state % STATES_SCREEN_REFRESH == 1) {
  prepareScreenLine4to6(true);
  refreshScreen();
 }
 //------flashing LED bit 7 to check the I2C communication------
 if(state % STATES_BLINK == 0) toggle = 1-toggle;
 sSerial+=" | toggle ="+String(toggle);
 pcf8574_out.setBit(7, toggle);
 //------button handling (reset by software)--------------------
 if(digitalRead(PIN_BUTTON)==0) {
  //.....button pressed (0).....................................
  if(reset_countdown<0) reset_countdown=STATES_BEFORE_RESET;
  else reset_countdown--;
  sSerial+="Resetcounter="+String(reset_countdown);
 } else {
  //.....button not pressed (1).................................
  reset_countdown=-1;
 }
 //......time for reset?........................................
 if(reset_countdown==0) {
  screen_.clear();
  screen_.useFontTitle();
  screen_.drawString(50,30,String("RESET"));
  esp_restart();
 } 
 //------For all components: replace STATE_NOW by current state-
 for(int i=0; i<RCOMP_NUM; i++) {
  if(aRcmd[i].stateToDo==STATE_NOW) {
   aRcmd[i].stateToDo=state;
   if(aRcmd[i].stateOffset!=STATE_NONE) {
    aRcmd[i].stateOffset=stm.add(aRcmd[i].stateOffset);
   }
  }
 }
 //------update all input values in aRcomp----------------------
 if(updateInputValues()) {                  // get input
  prepareScreenLine4to6(false);
  showLine(5, aScreenText[4]);
 }

 //======(3) process mqtt actions===============================
 #if _USE_WIFI_ == true
 if(bUseWiFi) {
  client.doLoop();                          // mqtt loop
  //=====(4) do, depending on the network access, ...===========
  if(client.isWiFiConnectedNew())    iConn=4;// "WiFi OK   ";
  if(client.isMQTTConnectedNew())    iConn=5;// "MQTT OK   ";
  if(client.isMQTTDisconnectedNew()) iConn=3;// "-No MQTT--";
  if(client.isWiFiDisconnectedNew()) iConn=2;// "-No WiFi--";
 } else {
  iConn=6;                                  // "unused WiFi"
 }
 //------show WLAN-/MQTT-connection status----------------------
 if(iConn!=iConnOld) {
  iConnOld=iConn;
  sSerial+=" | "+sConn[iConn];
  showLine(2,sConn[iConn]+" "+String(_SSID_));
 }
 #endif
 //======(5) do things after mqtt access========================
 //------act on a command---------------------------------------
 s1=actOnCmd(state);
 if(s1!="") sSerial+=" | "+s1;

 //======(6) do at the end of the loop ...======================
 DccAccessoryDecoder.loop();
  if(dccAddress>=0 && dccAddress<2048) {
  sSerial+=" | DCC Adresse="+String(dccAddress)+" Wert="+String(dccValue);
  dccAddress=-1;
 }

 uint32_t ms=stm.loopEnd();                   // state end
 //------print serial data--------------------------------------
 if(DEBUG_06) {
  Serial.print(sSerial+" | "); Serial.print(ms); 
  if(ms>STATE_DELAY) Serial.println("ms-Too long!!");
  else Serial.println("ms");
 }
}