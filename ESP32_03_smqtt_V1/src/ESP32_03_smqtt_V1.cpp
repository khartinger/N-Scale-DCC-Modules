//_____ESP32_03_smqtt_V1.cpp_____________________khartinger_____
// This program for a D1mini (or ESP32) shows
// .....
// In detail:
// 1. Connect to given WiFi and MQTT broker
// 2. Use "topic base" modul1 for mqtt communication
// 3. Automatic (build in) answers for messages 
//    -t modul1/get -m help
//    -t modul1/get -m version
//    -t modul1/get -m ip
//    -t modul1/get -m topicbase
//    -t modul1/get -m eeprom
// 4. Possibility to change the base topic:
//    -t modul1/set/topicbase -m new_topic_base
// 5. Possibility to delete stored base topic:
//    -t modul1/set/eeprom0 -m 
// Class SimpleMqtt extends class PubSubClient for easy use.
// All commands of the PubSubClient class can still be used.
//
// Hardware:
// 1. ESP32 D1 mini (or WeMos D1 mini)
//
// Note: When PubSubClient lib is installed,
//       delete PubSubClient files in directory src/simplemqtt
// Important: Example needs a MQTT-broker!
// Created by Karl Hartinger, March 21, 2024
// Changes:
// 2024-03-21 New
// Released into the public domain.
//#define D1MINI          1                 // ESP8266 D1mini +pro
#define ESP32D1         2                   // ESP32 D1mini

#include <Arduino.h>                       // String, int32_t
#include "wifi_config.h"                   // WiFi MQTT parameter
#include "src/simplemqtt/D1_class_SimpleMqtt.h"

#define  DEBUG_03       true                // OR false
#define  VERSION_03     "2024-03-21 ESP32_03_smqtt_V1"
#define  VERSION_03_1   "Version 2024-03-21"
#define  LANGUAGE_03    'e'                 // 'd' or 'e'
#define  LED_ON         1
#define  LED_OFF        0

//_______Global values and hardware_____________________________
int iLedValue=0;

//_______Global values for connection state_____________________
bool     bUseWiFi=_USE_WIFI_;
int      iConn=0;                          // WiFi unknown
int      iConnOld=0;                       // WiFi unknown
const    String sConn[]={"-unknown--", "connecting", 
         "-No WiFi--", "-No MQTT--", "WiFi OK   ", "MQTT OK   ",
        "Unused WiFi"};

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
  return p1;
 }
 //-------------------------------------------------------------
  if(sPayload=="version") {
  p1="{\"version\":\""; p1+= String(VERSION_03); p1+="\"}";
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

//_____SETUP____________________________________________________
void setup() {
 bool bRet;
 String s1="", s2="", s3="";
 //------Serial, just for debug---------------------------------
 if(DEBUG_03) {
  Serial.begin(115200);
  Serial.println("\nsetup(): --Start--");
 }
 //------hardware-----------------------------------------------
 pinMode(BUILTIN_LED, OUTPUT);              // build in led
 //------use WiFi and MQTT?-------------------------------------
 bUseWiFi=(_USE_WIFI_ ? true : false);
 //------Setup WiFi/MQTT client---------------------------------
#if _USE_WIFI_ == true
 if(bUseWiFi) {
  client.setLanguage(LANGUAGE_03);           //e=english,d=german
  client.setCallback(callback);              // mqtt receiver
  client.setTopicBaseDefault(TOPIC_BASE);    // topic base
  client.setWiFiWaitingTime(1000);           // set a short time (1s)
  client.setWiFiConnectingCounter(19);       // try 10s connecting
  s1="";
  client.setTopics(String(TOPIC_GET)+s1,String(TOPIC_SET)+s1,TOPIC_SUB,TOPIC_PUB);
  //client.setRetainedIndex("get",3,true);
  client.begin();                            // setup objects
  //------Show connecting procedure-----------------------------
  s1="WiFi "+String(_SSID_)+" connecting...";
  if(DEBUG_03) Serial.print("setup(): "+s1+String("\n"));
  client.connectingWiFiBegin();              // begin connecting
  int iUseWiFi=30;
  //.....waiting for WiFi connection............................
  do {
   bRet=client.connectingWiFi();             // try to connect
   digitalWrite(BUILTIN_LED, iLedValue);     // + toggle blue 
   if(DEBUG_03) Serial.print(".");
   iLedValue=1-iLedValue;                    // + build in led
   iUseWiFi--;                               // count down
  } while(!bRet && iUseWiFi>0);
  //.....END OF waiting for WiFi connection.....................
  if(DEBUG_03) Serial.println();
  if(iUseWiFi>0) {
   //----WiFi ok (no timeout)-----------------------------------
   digitalWrite(BUILTIN_LED, LED_ON);        // LED on = WiFi ok
   iConn=4;                                  // WiFi OK
   bUseWiFi=true;                            // use WiFi 
   s2=client.getsTopicBase();
   if(client.isMQTTConnected() || client.isMQTTConnectedNew()) {
    iConn=5;                                 // MQTT OK
    //----WiFi and MQTT OK: publish start info-------------------
    client.publish_P("info/start/setup",s2.c_str(),false);
    //client.bAllowMQTTStartInfo(false);     //NO mqtt (re)start info
    if(DEBUG_03) Serial.println(s2);
   }
  } else {
   //----WiFi timeout-------------------------------------------
   digitalWrite(BUILTIN_LED, LED_OFF);       // LED off = no WiFi
   iConn=2;                                  // NO WiFi
   bUseWiFi=false;                           // don´t use WiFi
  }
  s1="WiFi "+ sConn[iConn]+ " " + String(_SSID_);
 }
#else
 //------Dont use WiFi anyway-----------------------------------
 iConn=2;                                  // NO WiFi
#endif
 if(DEBUG_03) Serial.println("setup(): --Finished--\n");
}

//_____LOOP_____________________________________________________
void loop() {
 //======(1) make at the beginning of the loop ...==============
 String sSerial="";
 //======(2) do, independent on the network, ...================
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
 }
 if(iConn==5) digitalWrite(BUILTIN_LED, LED_ON); // LED on = ok
 else digitalWrite(BUILTIN_LED, LED_OFF);   // LED off = no MQTT
 #endif
 //======(5) do things after mqtt access========================
 //======(6) do at the end of the loop ...======================
 //------print serial data--------------------------------------
 if(DEBUG_03 && sSerial.length()>0) {
  Serial.println(sSerial); 
 }
 delay(100);
}