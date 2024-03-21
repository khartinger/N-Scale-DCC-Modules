//_____D1_class_SimpleMqtt.cpp_____________201208-210418_____
// The SimpleMqtt class is suitable for D1 mini (ESP8266)
// and ESP32 and extends the classes PubSubClient and
//  SimpleMqtt to make MQTT easy to use.
// * For this purpose a "base" topic (topicbase, default is
//   simplemqtt/default) is defined, which can be extended
//    by the following keywords:
// /get Request of the value specified in the payload
//      e.g. version query:
//      -t simplemqtt/default/get -m version
// /set Set a value e.g. 
//      -t simplemqtt/default/set/topicbase -m new_base
// * In response you get a MQTT message with the keyword /ret,
//    e.g. -t simplemqtt/default/ret/version -m 2020-12-08
// * Furthermore, all (arbitrary) topics defined in the constant
//   TOPIC_SUB are subscribed.
// * Furthermore, all commands of the PubSubClient class can 
//   still be used.
//
// In the user program the following things must be implemented:
// [1] Define the type of board (1=D1mini, 2=D1_ESP32)
//     #define  D1MINI         1            // ESP8266 D1mini +pro
// [2] Define the topic base
//     #define TOPIC_BASE "simplemqtt/default"
// [3] Define the topics the program should respond to 
//     for set/get/sub and pub requests
//     (key words comma separated or empty string) e.g.
//     #define TOPIC_GET  "help,version,ip,topicbase,eeprom,led"
//     #define TOPIC_SET  "topicbase,eeprom,led"
//     #define TOPIC_SUB  ""
//     #define TOPIC_PUB  ""
// [4] Create an object that will be used in the user program
//     SimpleMqtt client("ssid", "password", "mqttservername|ip");
// [5] Create a callback routine for incoming messages
//     void callback(char* topic, byte* payload, unsigned int length)
//     { client.callback_(topic, payload, length); }
// [6] Create functions to answer incomming MQTT requests
//     String simpleGet(String sPayload) { return String(""); }
//     String simpleSet(String sTopic, String sPayload) { return String(""); }
//     void   simpleSub(String sTopic, String sPayload) { }
// [7] Call method "doLoop()" in the main loop:
//     client.doLoop();
//     It MUST be called, otherwise no MQTT messages are processed!
//
// Note: If the PubSubClient class is registered (installed)
//       in the IDE, the PubSubClient files in the directory
//       src/simplemqtt should be deleted.
// Hardware: 
// (1) WeMos D1 mini OR D1 mini ESP32
// Important: Class needs a MQTT-broker to connect ;)
// Created by Karl Hartinger, December 08, 2020.
// Changes:
// 2020-12-16 add setLanguage (language_, SIMPLEMQTT_LANGUAGE)
// 2020-12-19 connectMQTT(): add line 2 if(!isWiFiConnected...
//            EEPROM: add eeprom...myData()
// 2021-01-03 add retained functionality
// 2021-01-10 TOPIC_MAX changed to 16
// 2021-04-18 add virtual to doLoop(), getsLocalIP(), 
//            constructor 6+7, replace delay(), set hostname
// Released into the public domain.

#include "D1_class_SimpleMqtt.h"

// *************************************************************
//    constructor & co
// *************************************************************

//_______constructor____________________________________________
SimpleMqtt::SimpleMqtt():PubSubClient(d1miniClient)
{
 ssid_=String(MQTT_SSID);
 pass_=String(MQTT_PASS);
 mqtt_=String(MQTT_SERVER);
 port_=MQTT_PORT;
 sTopicBaseDefault=SIMPLEMQTT_BASE;    // "path" of topics
 setup();
}

//_______constructor 2__________________________________________
SimpleMqtt::SimpleMqtt(char* ssid, char* pwd)
  :PubSubClient(d1miniClient)
{
 ssid_=String(ssid);
 pass_=String(pwd);
 mqtt_=String(MQTT_SERVER);
 port_=MQTT_PORT;
 sTopicBaseDefault=SIMPLEMQTT_BASE;    // "path" of topics
 setup();
}

/*
//_______constructor 3__________________________________________
SimpleMqtt::SimpleMqtt(char* ssid, char* pwd, 
  char* mqtt_server):PubSubClient(d1miniClient)
{
 ssid_=String(ssid);
 pass_=String(pwd);
 mqtt_=String(mqtt_server);
 port_=MQTT_PORT;
 sTopicBaseDefault=SIMPLEMQTT_BASE;    // "path" of topics
 setup();
}
*/

//_______constructor 4__________________________________________
SimpleMqtt::SimpleMqtt(char* ssid, char* pwd, 
  char* mqtt_server, char* topicbase)
  :PubSubClient(d1miniClient)
{
 ssid_=String(ssid);
 pass_=String(pwd);
 mqtt_=String(mqtt_server);
 port_=MQTT_PORT;
 sTopicBaseDefault=String(topicbase);
 setup();
}

//_______constructor 5__________________________________________
SimpleMqtt::SimpleMqtt(String sssid, String spwd, String smqtt_server)
:PubSubClient(d1miniClient)
{
 ssid_=sssid;
 pass_=spwd;
 mqtt_=smqtt_server;
 port_=MQTT_PORT;
 sTopicBaseDefault=SIMPLEMQTT_BASE;    // "path" of topics
 setup();
}

//_______constructor 6__________________________________________
SimpleMqtt::SimpleMqtt(String sssid, String spwd, String smqtt_server,
 String topicbase):PubSubClient(d1miniClient)
{
 ssid_=sssid;
 pass_=spwd;
 mqtt_=smqtt_server;
 port_=MQTT_PORT;
 sTopicBaseDefault=String(topicbase);  // "path" of topics
 setup();
}

//_______constructor 7__________________________________________
SimpleMqtt::SimpleMqtt(String sssid, String spwd, String smqtt_server, 
String topicbase, String clientname):PubSubClient(d1miniClient)
{
 ssid_=sssid;
 pass_=spwd;
 mqtt_=smqtt_server;
 port_=MQTT_PORT;
 sTopicBaseDefault=String(topicbase);  // "path" of topics
 setup();
 sMQTTClientName=clientname;
}

//_______setup (called by constructor)__________________________
void SimpleMqtt::setup()
{
 language_=SIMPLEMQTT_LANGUAGE;        // e=english, d=deutsch
 wifiWaitMsMax=TIMEOUT_WIFI_CONNECT_MS;
 wifiConnectingCounterMax=WIFI_CONNECTING_COUNTER;
 wifiConnectingCounter=0;              // start with connectingWiFiBegin
 sMyIP=NO_IP;                          // invalid IP
 startinfo_allow=STARTINFO_ALLOW;      // send mqtt start info
 randomSeed(micros());                 // start random numbers
 sMQTTClientName="D1_";                //
 sMQTTClientName+=String(random(0xffff), HEX);
 conState=CON_RESET;                   // WiFi + MQTT connection
 eepromSize_=EEPROM_SIZE;              //
 iGet=NOTHING_TODO;                    // no get answers to do
 iSet=NOTHING_TODO;                    // no set commands to do
 iRet=NOTHING_TODO;                    //
 iSub=NOTHING_TODO;                    //
 iPub=NOTHING_TODO;                    //
 numTopicGet=0;                        // no get topics yet
 numTopicSet=0;                        // no set topics yet
 numTopicSub=0;                        // no sub topics yet
 numTopicPub=0;                        // no pub topics yet
 //-----set all messages to retained = false--------------------
 for(int i=0; i<TOPIC_MAX; i++) {
  aRetainedGet[i]=false;
  aRetainedSet[i]=false;
  aRetainedPub[i]=false;
 }
 eeprom_=new EEPROMClass;              // eeprom object
 begin(0);                             // sets sTopicBase
}

//_______init objects___________________________________________
// read topic base from eeprom (or use default on error)
// iSource=0: called by constructor, 1: called by setup()
bool SimpleMqtt::begin(int iSource)
{
 int ret=true;
 //------(try to) read topic from eeprom------------------------
 int iResult;
 sTopicBase=eepromReadTopicBase(iResult);
 if(iResult<0) {
  sTopicBase=sTopicBaseDefault;        // "path" of topics
  if(DEBUG_MQTT) Serial.printf("begin(): topic base - use default %s\n",sTopicBase.c_str());
  ret=false;
 }
 if(DEBUG_MQTT) Serial.printf("begin(): topic base %s (from eeprom)\n",sTopicBase.c_str());
 //setupOta();
 return ret;
}

//_______init objects___________________________________________
bool SimpleMqtt::begin() { return begin(1); }
 
// *************************************************************
// setter and getter methods
// *************************************************************

//_______set language (default e=english; d=german)_____________
void SimpleMqtt::setLanguage(char language) { 
 if(language=='d' || language=='D') { language_='d'; return; }
 language_='e';
}

//_______set (used) EEPROM size_________________________________
void SimpleMqtt::setEepromSize(int eepromSize) {
 if(eepromSize>8 && eepromSize<=EEPROM_SIZE_MAX)
  eepromSize_= eepromSize; 
}

//_______Maximum Milliseconds to weit for WiFi to be connected__
void SimpleMqtt::setWiFiWaitingTime(int ms) 
{ if(ms>=100) wifiWaitMsMax=ms; }

//_______number of connectingWiFi() after connectingWiFiBegin__
void SimpleMqtt::setWiFiConnectingCounter(int number)
{ if(number>0) wifiConnectingCounterMax=number; }

  //_____allow/forbit sending mqtt start info___________________
void SimpleMqtt::allowMQTTStartInfo(bool allow)
{ startinfo_allow=allow; }


//_______get MQTT client state as string________________________
String SimpleMqtt::getsState()
{
 String s1;
 int client_state=state();
 s1="#"+String(client_state)+" ";
 switch(client_state)
 {
  case MQTT_CONNECTION_TIMEOUT:        // -4
   s1+="MQTT timeout connection"; break;
  case MQTT_CONNECTION_LOST:           // -3
   s1+="MQTT lost connection"; break;
  case MQTT_CONNECT_FAILED:            // -2
   s1+="MQTT failed to connect"; break;
  case MQTT_DISCONNECTED:              // -1
   s1+="MQTT disconnected"; break;
  case MQTT_CONNECTED:                 //  0
   s1+="MQTT connected"; break;
  case MQTT_CONNECT_BAD_PROTOCOL:      //  1
   s1+="MQTT bad protocol"; break;
  case MQTT_CONNECT_BAD_CLIENT_ID:     //  2
   s1+="MQTT bad client ID"; break;
  case MQTT_CONNECT_UNAVAILABLE:       //  3
   s1+="MQTT unavailable"; break;
  case MQTT_CONNECT_BAD_CREDENTIALS:   //  4
   s1+="MQTT bad credentials"; break;
  case MQTT_CONNECT_UNAUTHORIZED:      //  5
   s1+="MQTT unauthorized"; break;
 default: s1+="Unknown state"; break;
 }
 return s1;
}

//_______client name of WiFi network____________________________
String SimpleMqtt::getsSSID() { return ssid_; }

//_______read client IP address, return it as string____________
String SimpleMqtt::getsLocalIP() {
 if(DEBUG_MQTT) Serial.println("getsLocalIP() OHNE setupOta()");
 sMyIP=WiFi.localIP().toString();
 return sMyIP;
}

//_______return stored client IP as string______________________
String SimpleMqtt::getsMyIP() { return sMyIP; }


//_______get D1mini MAC address as string_______________________
String SimpleMqtt::getsMac()
{
 char cmac[18];                        // 12+5+1=18 bytes
 byte mac[6];                          // mac bytes
 WiFi.macAddress(mac);
 sprintf(cmac,"%02X:%02X:%02X:%02X:%02X:%02X",
  mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
 cmac[17]=0;
 String s1=String(cmac);
 return s1;
}

//_______get "path" for mqtt messages___________________________
String SimpleMqtt::getsTopicBase() { return sTopicBase; }

// *************************************************************
//    methods to declare mqtt topics
// *************************************************************

//_______set "path" for MQTT commands (+ eeprom)________________
// changes sTopicBase in RAM and EEPROM.
// Method does NOT subscribe new topics on MQTT broker! 
// (If you want this, use changeTopicBase() instead)
// return: 0=OK, 1=don't use EEPROM, 2=topic too long, 4=eeprom
//         write error, 8,16=eeprom read error
int SimpleMqtt::setTopicBase(String topicBasis)
{ 
 int ret=0;
 int len1=topicBasis.length();
 if(len1>=TOPIC_MAXLEN) 
 {
  if(DEBUG_MQTT) Serial.printf("setTopicBase(): failed! Topic %s too long",topicBasis.c_str());
  return 2;                            // topic too long
 }
 //------write new topic to eeprom------------------------------
 if(USE_EEPROM)
 {
  int len2=eepromWriteTopicBase(topicBasis);
  if(len1==len2) 
  {//....write to eeprom ok.....................................
   int iResult;
   String s1=eepromReadTopicBase(iResult);
   if(iResult<0) ret|=8;               // eeprom read error
  }
  else ret|=4;                         // eeprom write error
 }
 else ret|=1;                          // don't use eeprom
 if(DEBUG_MQTT) Serial.printf("setTopicBase(): %s ",topicBasis.c_str());
 if(DEBUG_MQTT) {if(ret>0) Serial.printf("failed! ret=%d\n",ret); else Serial.println("OK");} 
 return ret;
}

//_______set default "path" for MQTT commands (+ eeprom)________
// return: 0=OK, 1=don't use EEPROM, 2=topic too long, 4=eeprom
//         write error, 8,16=eeprom read error
int SimpleMqtt::setTopicBaseDefault(String topicBasisDefault)
{ 
 int ret=0;
 if(DEBUG_MQTT) Serial.printf("setTopicBaseDefault(): %s ",topicBasisDefault.c_str());
 if(sTopicBaseDefault==SIMPLEMQTT_BASE)
  sTopicBaseDefault=topicBasisDefault;
 if(sTopicBase==SIMPLEMQTT_BASE)
 {
  sTopicBase=topicBasisDefault;
 }
 else
 {
  sTopicBase=getsTopicBase();
 }
 if(DEBUG_MQTT) {if(ret>0) Serial.printf("failed! ret=%d\n",ret); else Serial.println("OK");} 
 return ret;
}

//_______set all (comma separated) topics_______________________
void SimpleMqtt::setTopics(String sAllGet, String sAllSet, 
                           String sAllSub, String sAllPub)
{
 setTopicGet(sAllGet);
 setTopicSet(sAllSet);
 setTopicSub(sAllSub);
 setTopicPub(sAllPub);
}

//_______set all get-topics as comma separated strings__________
int SimpleMqtt::setTopicGet(String sAllGet)
{
 numTopicGet=splitString(sAllGet, aTopicGet);
 return numTopicGet;
}

//_______set all set-topics as comma separated strings__________
int SimpleMqtt::setTopicSet(String sAllSet)
{
 numTopicSet=splitString(sAllSet, aTopicSet);
 return numTopicSet;
}

//_______set all sub(scribe)-topics as comma separated strings__
int SimpleMqtt::setTopicSub(String sAllSub)
{
 numTopicSub=splitString(sAllSub, aTopicSub);
 return numTopicSub;
}
 
//_______set all pub(lish)-topics as comma separated strings____
int SimpleMqtt::setTopicPub(String sAllPub)
{
 numTopicPub=splitString(sAllPub, aTopicPub);
 return numTopicPub;
}

//_______set all get-topics as comma separated strings__________
//       plus comma separated retained string (0=false, 1=true)
int SimpleMqtt::setTopicGet(String sAllGet, String sAllRetainedGet)
{
 splitString2Bool(sAllRetainedGet, aRetainedGet);
 numTopicGet=splitString(sAllGet, aTopicGet);
 return numTopicGet;
}

//_______set all set-topics as comma separated strings__________
//       plus comma separated retained string (0=false, 1=true)
int SimpleMqtt::setTopicSet(String sAllSet, String sAllRetainedSet)
{
 splitString2Bool(sAllRetainedSet, aRetainedSet);
 numTopicSet=splitString(sAllSet, aTopicSet);
 return numTopicSet;
}

//_______set all pub(lish)-topics as comma separated strings____
//       plus comma separated retained string (0=false, 1=true)
int SimpleMqtt::setTopicPub(String sAllPub, String sAllRetainedPub)
{
 splitString2Bool(sAllRetainedPub, aRetainedPub);
 numTopicPub=splitString(sAllPub, aTopicPub);
 return numTopicPub;
}

//_______set retained for index in a..Get|a..Set|a..Pub_________
boolean SimpleMqtt::setRetainedIndex(String sType, int index, boolean bRetained)
{
 if(sType=="get") {
  if(index>=0 && index<numTopicGet) {
   aRetainedGet[index]=bRetained;
   return true;
  }
 }
 if(sType=="set") {
  if(index>=0 && index<numTopicSet) {
   aRetainedSet[index]=bRetained;
   return true;
  }
 }
 if(sType=="pub") {
  if(index>=0 && index<numTopicPub) {
   aRetainedPub[index]=bRetained;
   return true;
  }
 }
 return false;
}

//_____ret all retained flags as string_______________________
String SimpleMqtt::getsRetainedAll()
{
 String s1="get: ";
 int i;
 for(i=0; i<numTopicGet; i++)
 {
  if(i>0) s1+=",";
  s1+=String(aRetainedGet[i]);
 }
 s1+="\nset: ";
 for(i=0; i<numTopicSet; i++)
 {
  if(i>0) s1+=",";
  s1+=String(aRetainedSet[i]);
 }
 s1+="\npub: ";
 for(i=0; i<numTopicPub; i++)
 {
  if(i>0) s1+=",";
  s1+=String(aRetainedPub[i]);
 }
 s1+="\n";
 return s1;
}



// *************************************************************
// methods for Wifi (WLAN)
// *************************************************************

//_______try to connect to WiFi, wait max. wifiWaitMsMax________
// return: true=connected (OK)
bool SimpleMqtt::connectWiFi() {
 wifiConnectingCounter--;              //connectings after begin
 if(wifiConnectingCounter<1)
 {
  wifiConnectingCounter=wifiConnectingCounterMax;
  if(!connectingWiFiBegin()) return false;
 }
 return connectingWiFi();
}

//_______start a new WiFi connection____________________________
// stopp a connection, if one existed before
// return: true=connected (OK)
bool SimpleMqtt::connectingWiFiBegin()
{
 disconnectWiFi();
 if(isWiFiConnected()) 
 {//-----error: could not disconnect WiFi-----------------------
  conState|=BIT_CONN_ERROR;                 // set error bit
  return false;
 }
 //delay(20);
 unsigned long _millisStart_=millis();
 while(millis()-_millisStart_<20) yield();  // wait 20ms
 if(DEBUG_MQTT) Serial.print("connectingWiFiBegin(): ");
#if defined(ESP8266) || defined(D1MINI)
 if(DEBUG_MQTT) Serial.print("mode() OK - ");  
 WiFi.mode(WIFI_STA);                  // D1mini is station
 WiFi.hostname(sMQTTClientName.c_str());
 wifi_station_set_hostname(sMQTTClientName.c_str());
#endif
#if defined(ESP32) || defined(ESP32D1)
 // WiFi.config(ip, gateway, subnet);  // skipp for dhcp
 WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE); // required to set hostname properly
 WiFi.setHostname(sMQTTClientName.c_str());
 WiFi.mode(WIFI_STA);                  // uC is station
#endif
 // WiFi.config(ip, gateway, subnet);  // skipp for dhcp
 //wifi_station_set_auto_connect(true);
 //wifi_station_set_hostname(sWifiHostname.c_str());
 //WiFi.hostname(ota_hostname_.c_str());
 //wifi_station_set_hostname(ota_hostname_.c_str());
 //-----try to connect to WiFi (access point)-------------------
 String s1=String(ssid_);              // WLAN name
 //WiFi.begin(ssid_, pass_);           // start connecting!
 WiFi.begin((char*) ssid_.c_str(), (const char *)pass_.c_str());    // start connecting!
 if(DEBUG_MQTT) Serial.println("WiFi connecting to "+s1); 
 if(isWiFiConnected()) 
 {//-----normally no such quick connection...-------------------
  sMyIP=getsLocalIP();                      // get ip
 }
 else
 {//-----normal case--------------------------------------------
  sMyIP=NO_IP;                              // no IP
 }
 return true;                               // connection startet
}

//_______wait for WiFi connection to be established_____________
// Sets smyIP
// return: true=connection established
bool SimpleMqtt::connectingWiFi() { return connectingWiFi(1); }

//_______wait n times for WiFi connection to be established_____
// Waiting time = wifiWaitMsMax * attempts
// Sets smyIP
// return: true=connection established
bool SimpleMqtt::connectingWiFi(int attempts)
{
 //unsigned long nextMillis;
 //------check connection---------------------------------------
 if(isWiFiConnected()) 
 {
  sMyIP=getsLocalIP();                      // get ip
  return true;                              // connection state
 }
 //------try to connect-----------------------------------------
 if(attempts<1) attempts=1;                 //try min. 1x
 int i=wifiWaitMsMax/100;                   //100ms waiting step
 if(i<1) i=1;                               // minimum 100ms
 i=i*attempts;                              // counter (100ms steps)
 if(DEBUG_MQTT) Serial.print("connectingWiFi() - Waiting for connect ");
 
 while(!isWiFiConnected() && (i>0))
 {
  //delay(100);                             // wait 100ms
  unsigned long _millisStart_=millis();
  while(millis()-_millisStart_<100) yield();// wait 100ms
  i--;                                      // decrement counter
  if(DEBUG_MQTT){Serial.print("."); if(i%50==0) Serial.println("");}
 }
 //------connected to WLAN (access point)?---------------------
 if(i<1)                                    // counter=minimum
 { 
  if(!isWiFiConnected())
  {//----not connected to WLAN----------------------------------
   if(DEBUG_MQTT) Serial.println("connectingWiFi(): time-out!");
   sMyIP=NO_IP;                             // no IP
   return false;                            // connection state
  }
 }
 //------success WiFi new connection/reconnect-----------------
 sMyIP=getsLocalIP();                       // get ip
 if(DEBUG_MQTT) Serial.println("\nconnectingWiFi(): New connection to "+String(ssid_)+", IP="+sMyIP+" - OK");
 return true;                               // connection state
}

//_______disconnect a WiFi connection___________________________
// return: true=disconnected, false=could not disconnect Wifi
bool SimpleMqtt::disconnectWiFi()
{
 int i=20;
 //bool bWiFiLast=((conState&BIT_WIFI_LAST)>0) ? true : false;
 //------(try to) disconnect------------------------------------
 while(isWiFiConnected() && (i>0))
 {
  i--;                                      // increment trials
  WiFi.disconnect();                        // disconnect WLAN 
  //delay(50);                                // wait a litte bit
  unsigned long _millisStart_=millis();
  while(millis()-_millisStart_<50) yield(); // wait 50ms
 }
 //------disconnected-------------------------------------------
 if(WiFi.status()==WL_DISCONNECTED) {
  //conState|=BIT_WIFI_DIS;
  if(DEBUG_MQTT) Serial.println("disconnectWiFi(): OK");
  return true;
 }
 //------error while disconnecting------------------------------
 if(!isWiFiConnected()) {
  //conState|=BIT_WIFI_DIS;
  if(DEBUG_MQTT) Serial.println("disconnectWiFi(): No Wifi - OK");
  return true;
 }
 //------other error/state--------------------------------------
 if(DEBUG_MQTT){Serial.print("disconnectWiFi(): Error #");Serial.println(WiFi.status());}
 conState|=BIT_CONN_ERROR;                  // set error bit
 return false;
}

//_______is client connected to WiFi (WLAN)?____________________
// uses WiFi.status()
// If there was no change of status, the status word is not
// changed. The current status becomes the old status.
// return: true = WiFi connection ok
bool SimpleMqtt::isWiFiConnected() {
 //------get last two WiFi states (now and last)----------------
 bool bWiFiNow=(WiFi.status()==WL_CONNECTED);
 bool bWiFiLast=((conState&BIT_WIFI_LAST)>0) ? true : false;
 if(bWiFiNow && (!bWiFiLast))
 {//-----last 0 -> 1 now: new connection!-----------------------
  conState |= BIT_WIFI_CON;            // set new connection bit
  conState &= (~BIT_WIFI_DIS);         // forget disconnect
 }
 if((!bWiFiNow) && bWiFiLast)
 {//-----last 1 -> 0 now: connection lost!----------------------
  conState |= BIT_WIFI_DIS;            // set disconnection bit
  conState &= (~BIT_WIFI_CON);         // forget connect bit
  //.....WiFi lost -> MQTT connection lost......................
  conState &= (~BIT_MQTT_LAST);        // MQTT-state is no MQTT
  conState |= BIT_MQTT_DIS;            // MQTT lost
  conState &= (~BIT_MQTT_CON);         // no MQTT connection
  //.....next connecting with ConnectingBegin().................
  wifiConnectingCounter=0;
 }
 //------make the current status the old status-----------------
 if(bWiFiNow) conState|=BIT_WIFI_LAST; // set last WiFi state
 else conState&=(~BIT_WIFI_LAST);      // clear last WiFi state
 return bWiFiNow;
}

//_______is WiFi new connected?_________________________________
// just read conState, NO WiFi.status() call!
// return: true = new connected
bool SimpleMqtt::isWiFiConnectedNew()
{
 bool wifiConNew = ((conState&BIT_WIFI_CON)>0) ? true : false;
 conState&=(~BIT_WIFI_CON);            // connection bit read
 return wifiConNew;
}

//_______is WiFi new disconnected?______________________________
// just read conState, NO WiFi.status() call!
// return: true = new disconnected
bool SimpleMqtt::isWiFiDisconnectedNew()
{
 bool wifiDisNew = ((conState&BIT_WIFI_DIS)>0) ? true : false;
 conState&=(~BIT_WIFI_DIS);            // disconnection bit read
 return wifiDisNew;
}

//_______get WiFi signal strength (or ? on error)_______________
// return: signal strength or ? on error
String SimpleMqtt::getsSignal() 
{
 if(isWiFiConnected()) return String(WiFi.RSSI());
 return String("?");
}

// *************************************************************
// methods for mqtt connection
// *************************************************************

//_______wait for WiFi connection, then connect to MQTT server__
// return: true=MQTT connected
bool SimpleMqtt::connectWiFiMQTT()
{
 //------is client NOT connected to WiFi?-----------------------
 //if(WiFi.status()!=WL_CONNECTED)
 if(!isWiFiConnected())
 {//-----try to (re)connect to WiFi-----------------------------
  if(!connectWiFi()) return false;     // not connected
 }
 return connectMQTT();                 // try to connect
}

//_______try to connect to MQTT server__________________________
// return: true = connected to MQTT broker
//         details in connState
bool SimpleMqtt::connectMQTT()
{
 int iRet=0;
 if(!isWiFiConnected()) return false;
 //------is client already connected to MQTT broker?------------
 if(isMQTTConnected()) return true;
 sMyIP=WiFi.localIP().toString();
 //------(try to) connect to MQTT server (normal case)----------
 for(int i=3; i>0; i--)
 {//-----try to connect to MQTT server--------------------------
  setServer(mqtt_.c_str(), port_);
  if(PubSubClient::connect(sMQTTClientName.c_str()))
  {//----connected to MQTT server: subscribe topics-------------
   if(DEBUG_MQTT) Serial.println("connectMQTT(): connect");
   if(!isMQTTConnected()) return false;
   iRet=changeSubscribe(sTopicBase, sTopicBase); // 
   if(iRet==0 || iRet==32)
   {//---subscribe all topics is OK-----------------------------
    if(startinfo_allow)
    {
     String s1="{\"topicbase\":\"";
     s1+=sTopicBase+"\",\"IP\":\""+sMyIP+"\"}";
     publish(STARTINFO_TOPIC, s1.c_str());   //start message
    }
    if(DEBUG_MQTT) Serial.println("connectMQTT(): OK");
    return true;                       // connected!
   }
   else
   {//---could not subscribe topics-----------------------------
    if(DEBUG_MQTT) Serial.println("connectMQTT(): Subscribe Error");
    PubSubClient::disconnect();        // try again
   }
  }
  if(DEBUG_MQTT) Serial.print("connectMQTT() failed: ");
  if(DEBUG_MQTT) Serial.print(getsState().c_str());
  if(DEBUG_MQTT) Serial.println(". Wait and try again...");
  //delay(MQTT_RECONNECT_MS);            // wait a little bit
  unsigned long _millisStart_=millis();  // wait a little bit
  while(millis()-_millisStart_<MQTT_RECONNECT_MS) yield();
 }
 //------not connected to MQTT broker---------------------------
 if(DEBUG_MQTT) Serial.println("connectMQTT(): MQTT error!");
 conState&=(~BIT_MQTT_LAST);
 conState&=(~BIT_MQTT_CON);            // MQTT not connected
 return false;                         // no connection
}

//_______is mqtt connection ok? (no: reconnect)_________________
// return: true=connected to MQTT server
// MUST always be called in main loop (for receive!)
bool SimpleMqtt::checkMQTT()
{
 if(!isMQTTConnected()) return false;  // no MQTT connection
 //------if connected to broker, do loop function---------------
 PubSubClient::loop();                // message received?
 return true;
}

//_______is mqtt connection ok?_________________________________
// uses WiFi.status() and PubSubClient::connected()
// If there was no change of status, the status word is not
// changed. The current status becomes the old status.
// return: true = MQTT connection ok
bool SimpleMqtt::isMQTTConnected()
{
 if(!isWiFiConnected()) return false;  // NOT connected to WiFi
 //------get last two MQTT states (now and last)----------------
 bool bMqttNow =PubSubClient::connected();
 bool bMqttLast=((conState&BIT_MQTT_LAST)>0) ? true : false;
 if(bMqttNow && (!bMqttLast))
 {//-----last 0 -> 1 now: new connection!-----------------------
  conState |= BIT_MQTT_CON;            // set new connection bit
  conState &= (~BIT_MQTT_DIS);          // forget disconnect
 }
 if((!bMqttNow) && bMqttLast)
 {//-----last 1 -> 0 now: connection lost!----------------------
  conState |= BIT_MQTT_DIS;            // set disconnection bit
  conState &= (~BIT_MQTT_CON);         // forget connect bit
 }
 //------make the current status the old status-----------------
 if(bMqttNow) conState|=BIT_MQTT_LAST; // set last WiFi state
 else conState&=(~BIT_MQTT_LAST);      // clear last WiFi state
 return bMqttNow;
}


//_______is MQTT new connected?_________________________________
// just read conState, NO PubSubClient::connected() call!
// return: true = new connection to MQTT broker
bool SimpleMqtt::isMQTTConnectedNew()
{
 bool mqttConNew = ((conState&BIT_MQTT_CON)>0) ? true : false;
 conState&=(~BIT_MQTT_CON);            // connection bit read
 return mqttConNew;
}

//_______is MQTT new disconnected?______________________________
// just read conState, NO PubSubClient::connected() call!
// return: true = new disconnection from MQTT broker
bool SimpleMqtt::isMQTTDisconnectedNew()
{
 bool mqttDisNew = ((conState&BIT_MQTT_DIS)>0) ? true : false;
 conState&=(~BIT_MQTT_DIS);            // disconnection bit read
 return mqttDisNew;
}

// *************************************************************
//    mqtt (main) loop control
// *************************************************************

//_______control mqtt in main loop (true: with reconnect)_______
// calls checkMQTT(); that MUST be called every loop
bool SimpleMqtt::doLoop(void) { return(doLoop(true)); }
  
//_______control mqtt in main loop (without reconnect)__________
// calls checkMQTT(); that MUST be called every loop
// uses public functions 
// String mqttloopGet(String sPayload)
// String mqttloopSet(String sTopic, String sPayload)
// void   mqttloopSub(String sTopic, String sPayload)
// return: true=PubSubClient loop() done
bool SimpleMqtt::doLoop(bool tryToReconnect)
{
 bool bRet;
 //======SECTION 1: action at the beginning of loop=============
 //------process set requests-----------------------------------
 if(iSet>0)
 {
  for(int i=0; i<numTopicSet; i++)
  {
   if((iSet&(1<<i))>0)
   {
    String s1=simpleSet(aTopicSet[i],aPayloadSet[i]);
    if(s1.length()>0)
    {
     aPayloadSet[i]=s1;
     iRetSet|=(1<<i);
    }
    iSet&=(~(1<<i));
   }  
  }
 }
 //------process sub requests-----------------------------------
 if(iSub>0)
 {
  for(int i=0; i<numTopicSub; i++)
  {
   if((iSub&(1<<i))>0)
   {
    simpleSub(aTopicSub[i],aPayloadSub[i]);
    iSub&=(~(1<<i));
   }  
  }
 }
 //======SECTION 2: check for WiFi and MQTT connection==========
 bRet=checkMQTT();                // MUST be called for receive!
 if(bRet)
 {//-----connected to MQTT server-------------------------------
  if(iGet>0) createGetAnswer();
 }
 else
 {//-----not connected! Try to connect--------------------------
  if(tryToReconnect) connectWiFiMQTT();
 }
 //------send messages------------------------------------------
 if((iRet>0)||(iRetSet>0)||(iPub>0)) sendRet();
 return bRet;                               // result checkMQTT
}

// *************************************************************
//    internal callback method
//    MUST be called by global callback routine!
// *************************************************************

//_______internal callback method_______________________________
// MQTT: inspect all subscribed incoming messages
// * If "get" topic is valid, the appropriate bit is set in iGet
// * If "set" topic is valid, the appropriate bit is set in iSet
//   and the payload is copied to sSetPayload[]
// * Special topics are specially treated
void SimpleMqtt::callback_(char* topic, byte* payload, unsigned int length)
{
 int i=-1;
 //======SECTION 1: convert payload to array (and show message)=
 //------build get/set/...topic---------------------------------
 int lenTopic=strlen(topic);
 char cTopic[5+lenTopic];
 sprintf(cTopic,"%s/get",sTopicBase.c_str()); // "get" topic
 char cPayload[length+1];                   //char-array payload
 strncpy(cPayload,(char*)payload,length);   // copy payload bytes
 cPayload[length]=0;                        // set end char
 if(DEBUG_MQTT) Serial.printf("callback_(): topic '%s': %s\n",topic,cPayload);
 //======SECTION 2: GET request -> trigger answer===============
 if(strcmp(topic,cTopic)==0)
 {//-----check for valid get request (see array sGet[]----------
  for(i=0; i<numTopicGet; i++) {
   if(strcmp(aTopicGet[i].c_str(), cPayload)==0)
   { 
    if(DEBUG_MQTT) Serial.printf("callback_(): ==> MQTT process get '%s'\n",cPayload);
    iGet|=(1<<i); 
    i=-1;                               // finish for
    break; 
   }
  }
  if(DEBUG_MQTT && i!=-1) Serial.printf("callback_(): *** no valid get payload found ***\n");
 }
 //======SECTION 3: SET request -> trigger action===============
 // do action in main loop to save time in callback function (!)
 sprintf(cTopic,"%s/set/",sTopicBase.c_str()); // "set" topic
 lenTopic=strlen(cTopic);              // len of ".../set/"
 if(strncmp(topic,cTopic,lenTopic)==0) // check first part
 {//-----correct first part of set topic found------------------
  // check for valid 2nd part of set request (= aTopicSet[])
  char* settype=topic;                 // point to topic begin
  settype+=lenTopic;                   // point to topic command
  for(i=0; i<numTopicSet; i++) {
   if(strcmp(settype,aTopicSet[i].c_str())==0)
   {
    if(DEBUG_MQTT) Serial.printf("callback_(): ==> MQTT command set %s=%s\n",(aTopicSet[i]).c_str(),cPayload);
    iSet|=(1<<i);                      // trigger get request
    aPayloadSet[i]=String(cPayload);   // save payload
    i=-1;                              // finish for
    break;                             // set-command?
   }
  }
  if(DEBUG_MQTT && i!=-1) Serial.printf("callback_(): *** no valid set topic found ***\n");
 }
 //======SECTION 4: special MQTT (input) messages===============
 for(i=0; i<numTopicSub; i++) {
  if(strcmp(aTopicSub[i].c_str(),topic)==0){
   if(DEBUG_MQTT) Serial.printf("callback_(): ==> MQTT special: topic %s, payload %s\n",(aTopicSub[i]).c_str(),cPayload);
   iSub|=(1<<i);                       // trigger get request
   aPayloadSub[i]=String(cPayload);    // save payload
   break;                              // set-command?
  }
 }
}

// *************************************************************
//    mqtt working methods
// *************************************************************

//_______set new topic base, unsubscribe old topics, subscribe
//       new topics_____________________________________________
// return: true: topics changes, false: error
bool SimpleMqtt::changeTopicBase(String newBase)
{
 return changeTopicBase(sTopicBase, newBase);
}

//_______set new topic base, unsubscribe old topics, subscribe
//       new topics_____________________________________________
// return: true: topics changes, false: error
bool SimpleMqtt::changeTopicBase(String oldBase, String newBase)
{
 String tempBase=sTopicBase;
 int  iRet=setTopicBase(newBase);      //try to change topic name
 if(iRet!=0) 
 {//-----could not change sTopicBase----------------------------
  sTopicBase=tempBase;                 // reset old value
  return false;                        // change failed
 }
 sTopicBase=newBase;
 iRet=changeSubscribe(oldBase, newBase);
 //iRet: 0=OK,16=no mqtt-server,32=unsubscribe/64=subscribe error
 if(iRet==0)
 {
  if(DEBUG_MQTT) Serial.println("changeTopicBase(): OK");
  return true;
 }
 if((iRet&32)==32)
 {
  if(DEBUG_MQTT) Serial.println("changeTopicBase(): OK (unsubscribe failed)");
  return true;
 }
 if(DEBUG_MQTT) Serial.println("changeTopicBase(): failed");
 return false;
}

//_______prepare to send a message with topic out of aPayloadPub
void SimpleMqtt::sendPubIndex(int index, String payload)
{
 if((index<0) || (index>=TOPIC_MAX)) return;
 iPub|=(1<<index);
 aPayloadPub[index]=payload;
}

//_______force (simulate) a get-, set-, sub- or pub-message_____
bool SimpleMqtt::simpleMqttDo(String type, String topic, String payload)
{
 //------get message--------------------------------------------
 if(type=="get") {
  for(int i=0; i<numTopicGet; i++) {
   if(topic==aTopicGet[i]) {
    iGet|=(1<<i);
    return true;
   }
  }
  return false;
 }
 //------set message--------------------------------------------
 if(type=="set") {
  for(int i=0; i<numTopicSet; i++) {
   if(topic==aTopicSet[i]) {
    aPayloadSet[i]=payload;
    iSet|=(1<<i);
    return true;
   }
  }
  return false;
 }
 //------sub message--------------------------------------------
 if(type=="sub") {
  for(int i=0; i<numTopicSub; i++) {
   if(topic==aTopicSub[i]) {
    aPayloadSub[i]=payload;
    iSub|=(1<<i);
    return true;
   }
  }
  return false;
 }
 //------pub message--------------------------------------------
 if(type=="pub") {
  for(int i=0; i<numTopicPub; i++) {
   if(topic==aTopicPub[i]) {
    aPayloadPub[i]=payload;
    iPub|=(1<<i);
    return true;
   }
  }
  return false;
 }
 return false;
}

//_______force (simulate) a get-, set-, sub- or pub-message_____
// same as simpleMqttDo()
bool SimpleMqtt::forceXXXAnswer(String type, String topic, String payload)
{
  return simpleMqttDo(type, topic, payload);
}


// *************************************************************
//     connection state
// *************************************************************

//_______is error bit set?______________________________________
bool SimpleMqtt::isConnectError() {
 bool bConErr=(conState&BIT_CONN_ERROR)>0 ? true : false;
 return bConErr;
}

//_______clear connection error bit_____________________________
void SimpleMqtt::resetConnectError() {conState&=(~BIT_CONN_ERROR);}

//_______convert conState to unsigned long number_______________
unsigned long SimpleMqtt::getConState() {return conState;}

//_______convert conState to HEX string_________________________
String SimpleMqtt::getConStateHex() {
 unsigned long ul1=conState; 
 int i=sizeof(unsigned long);
 char buffer[i+1];
 buffer[i]=0;
 while(i>0)
 {
  i--;
  uint8_t ch=(ul1 & 0x0F) + '0';
  if(ch>'9') ch+=7;
  buffer[i]=ch;
  ul1>>=4;
 }
 String s1=String(buffer);
 return s1;
}

// *************************************************************
//    helper methods
// *************************************************************

//_______subscribe get-, set-, sub-topic________________________
// return: true=OK, false on error
bool SimpleMqtt::subscribeAllTopics(){
 return subscribeAllTopics(sTopicBase); 
}

//_______subscribe get-, set-, sub-topic for given topic________
// return: true=OK, false on error
bool SimpleMqtt::subscribeAllTopics(String topicBasis)
{
 bool ret=true;
 char ca[7+topicBasis.length()];
 if(!(PubSubClient::connected())) return false;
 sprintf(ca,"%s/get",topicBasis.c_str());// "get" topic
 ret&=subscribe(ca);
 sprintf(ca,"%s/set/#",topicBasis.c_str());// "get" topic
 ret&=subscribe(ca);
 for(int i=0; i<numTopicSub; i++)
  ret&=subscribe(aTopicSub[i].c_str());
 return ret;
}

//_______unsubscribe get-, set-, sub-topic______________________
// return: true=OK, false on error
bool SimpleMqtt::unsubscribeAllTopics()
{ return unsubscribeAllTopics(sTopicBase); }

//_______unsubscribe get-, set-, sub-topic______________________
// return: true=OK, false on error
bool SimpleMqtt::unsubscribeAllTopics(String topicBasis)
{
 bool ret=true;
 int len=topicBasis.length();
 char ca[7+len];
 if(!(PubSubClient::connected())) return false;
 sprintf(ca,"%s/get",topicBasis.c_str());   // "get" topic
 //ca[4+len]=0;
 ret&=unsubscribe(ca);
 sprintf(ca,"%s/set/#",topicBasis.c_str()); // "get" topic
 //ca[6+len]=0;
 ret&=unsubscribe(ca);
 for(int i=0; i<numTopicSub; i++)
  ret&=unsubscribe(aTopicSub[i].c_str());
 return ret;
}

//_______unsubscribe old, subscribe new topic base______________
// return: 0=OK, 16=no mqtt server, 32=unsubscribe error,
//         64=subscribe error
int SimpleMqtt::changeSubscribe(String oldTopic, String newTopic)
{
 int ret=0;                                 // no error
 if(DEBUG_MQTT) Serial.print("changeSubscribe(): ");
 //------sTopicBase is new, try to update MQTT-server-----------
 if(PubSubClient::connected())
 {
   
  //-----unsubscribe old topics---------------------------------
  if(!unsubscribeAllTopics(oldTopic)) ret|=32;// unsubscribe error
  //-----subscribe new topics-----------------------------------
  if(!subscribeAllTopics(newTopic)) 
  {
   if(DEBUG_MQTT) Serial.printf("Could not subscribe Topics. ");
   ret|=64; // subscribe error
   if(startinfo_allow)
   {
    String s1="{\"pos=\":\"changeSubscribe\",\"topicbase\":\"";
    s1+=sTopicBase+"\",\"IP\":\""+getsMyIP()+"\"}";
    publish(STARTINFO_TOPIC, s1.c_str());   //start message
   }
  }
 }
 else
 {
  ret=16;
  if(DEBUG_MQTT) Serial.printf("failed! MQTT broker not connected. ");
 }
 if(DEBUG_MQTT) {if(ret>0) Serial.printf("ret=%d\n",ret); else Serial.println("OK");}
 return ret;                                // 
}

//_______send answers to get-/set-requests______________________
void SimpleMqtt::sendRet()
{
 //------send (ret) answers for get request---------------------
 for(int i=0; i<numTopicGet; i++)
 {
  if((iRet&(1<<i))>0) 
  {
   String t1=sTopicBase;
   t1+="/ret/";
   t1+=aTopicGet[i];
   if(publish(t1.c_str(),aPayloadRet[i].c_str(), aRetainedGet[i]))
    iRet&=(~(1<<i));
  }
 }
 //------send (retset) answers for set requests-----------------
 for(int i=0; i<numTopicSet; i++)
 {
  if((iRetSet&(1<<i))>0) 
  {
   String t1=sTopicBase;
   t1+="/ret/";
   t1+=aTopicSet[i];
   if(publish(t1.c_str(),aPayloadSet[i].c_str(), aRetainedSet[i]))
   {
    iRetSet&=(~(1<<i));
    iSet&=(~(1<<i));
   }
  }
 }
 //------send publish message-----------------------------------
 for(int i=0; i<numTopicPub; i++)
 {
  if((iPub&(1<<i))>0) 
  {
   if(publish(aTopicPub[i].c_str(),aPayloadPub[i].c_str(), aRetainedPub[i]))
   {
    iPub&=(~(1<<i));
   }
  }
 }
 //------delete all requests anyway-----------------------------
 iRet=0; iRetSet=0; iPub=0;
}

//_______convert Array to json format___________________________
String SimpleMqtt::jsonArray(String what)
{
 int num=0;
 String s1="{";
 if(what=="get")
 { 
  num=numTopicGet;
  s1+="\"num\":";
  s1+=String(num);
  s1+=",\"val\":{";
  for(int i=0; i<num; i++)
  {
   if(i>0) s1+=",";
   s1+="\""+aTopicGet[i]+"\"";
  }
 }
 s1+="}}";
 return s1;
}

//_______split string to array 1________________________________
int SimpleMqtt::splitString(String str, String aStr[])
{
 return splitString(str, aStr, ",", TOPIC_MAX);
}

//_______split string to array 2________________________________
int SimpleMqtt::splitString(String str, String aStr[], String delimiter)
{
 return splitString(str, aStr, delimiter, TOPIC_MAX);
}

//_______split string to array 3_________________________________
// return: array aStr and number of elements in array aStr
int SimpleMqtt::splitString(String str, String aStr[], 
 String delimiter, int imax)
{
 int anz=0;
 int len1=delimiter.length();
 if(len1<1) return anz; 
 if(str.length()<1) return anz;
 int pos1=0,pos2=-1;
 while((pos2=str.indexOf(delimiter,pos1))>=0)
 {
  aStr[anz++]=str.substring(pos1,pos2);
  if(anz>=imax) return anz;
  pos1=pos2+len1;
 }
 aStr[anz++]=str.substring(pos1);
 return anz;
}

//_______split string to boolean array 1 (0=false, 1=true)______
int SimpleMqtt::splitString2Bool(String str, boolean bStr[])
{
 return splitString2Bool(str, bStr, ",", TOPIC_MAX);
}

//_____split string to boolean array 2 (0=false, 1=true)______
int SimpleMqtt::splitString2Bool(String str, boolean bStr[], String delimiter)
{
 return splitString2Bool(str, bStr, delimiter, TOPIC_MAX);
}

//_____split string to boolean array 3 (0=false, 1=true)______
int SimpleMqtt::splitString2Bool(String str, boolean bStr[], String delimiter, int imax)
{
 int anz=0;
 int len1=delimiter.length();
 if(len1<1) return anz; 
 if(str.length()<1) return anz;
 int pos1=0,pos2=-1;
 while((pos2=str.indexOf(delimiter,pos1))>=0)
 {
  String s1=str.substring(pos1,pos2);
  s1.replace(" ","");
  //Serial.print("|"+s1);
  bStr[anz++] = (s1=="1");                  // true or false
  if(anz>=imax) return anz;
  pos1=pos2+len1;
 }
 String s2=str.substring(pos1);
 s2.replace(" ","");
 //Serial.print("|"+s2);
 bStr[anz++] = (s2=="1");                   // true or false
 //Serial.println();
 return anz;
}

// *************************************************************
//     internal methods
// *************************************************************

//_______generate get answers in array aPayloadRet[]____________
// uses  : iGet, iRet, aPayloadRet[], numTopicXXX, aTopicXXX[]
//         XXX = Get, Set, Sub, Pub (for help answer ;)
// calls : external function doGetAnswer()
// result: answers in array aPayloadRet[]
// called by doLoop()
void SimpleMqtt::createGetAnswer()
{
 if(iGet>0)
 {
  for(int i=0; i<numTopicGet; i++)
  {
   if((iGet&(1<<i))>0)
   {
    //-----"automatic" get answers-----------------------------
    if((aTopicGet[i]=="help") || (aTopicGet[i]=="?") ||
      ((language_=='d') && (aTopicGet[i]=="hilfe"))) {
     String p1="\r\nget: ";                 // list all get
     for(int i=0; i<numTopicGet; i++) p1+=aTopicGet[i]+"|";
     p1+="\r\nset: ";                       // list all set
     for(int i=0; i<numTopicSet; i++) p1+=aTopicSet[i]+"|";
     p1+="\r\nsub: ";                        // list all sub
     for(int i=0; i<numTopicSub; i++) p1+=aTopicSub[i]+"|";
     p1+="\r\npub: ";                        // list all pub
     for(int i=0; i<numTopicPub; i++) p1+=aTopicPub[i]+"|";
     p1+="\r\n";
     aPayloadRet[i]=p1; 
     iRet|=(1<<i);
    }
    if(aTopicGet[i]=="version") {
     String p1="SimpleMqtt Version ";
     p1+=SIMPLEMQTT_VERSION;
     aPayloadRet[i]=p1;
     iRet|=(1<<i);
    }
    if(aTopicGet[i]=="ip") {
     aPayloadRet[i]=getsMyIP(); 
     iRet|=(1<<i);
    }
    //-----user answers [from global function doGetAnswer()]----
    String s1=simpleGet(aTopicGet[i]);
    if(s1.length()>0)
    {
     if((aTopicGet[i]=="help") || (aTopicGet[i]=="?") ||
      ((language_=='d') && (aTopicGet[i]=="hilfe")))
     {
      if(s1.charAt(0)=='+') 
       s1=aPayloadRet[i]+s1.substring(1);    // append to help 
     }
     aPayloadRet[i]=s1;
     iRet|=(1<<i);
    }
   }
  }
  iGet=0;
 }
}

// *************************************************************
//     methods for eeprom read/write
// *************************************************************

//_______eeprom return value as string__________________________
// iResult >=0: OK (length),
// Read error:  -1 EEPROM not allowed, -2 wrong begin, 
//              -3 wrong length, -4 read error, -5 wrong end
// Write error: -11 EEPROM not allowed, -12 sData too long, 
//              -13 eeprom size too small, -14 write error
String SimpleMqtt::getsEepromStatus(int iResult)
{
 if(iResult>=0) return "OK";
 String s1="";
 if(language_=='d')
 {
  switch(iResult)
  {
   case  -1: s1="EEPROM read: EEPROM-Verwendung nicht erlaubt"; break;
   case  -2: s1="EEPROM read: falsche Startsequenz"; break;
   case  -3: s1="EEPROM read: falsche Datenlaenge"; break;
   case  -4: s1="EEPROM read: Lesefehler"; break;
   case  -5: s1="EEPROM read: falsche Endesequenz"; break;
   case -11: s1="EEPROM write: EEPROM-Verwendung nicht erlaubt"; break;
   case -12: s1="EEPROM write: Daten zu lang"; break;
   case -13: s1="EEPROM write: EEPROM zu klein"; break;
   case -14: s1="EEPROM write: Schreibfehler"; break;
   default:  s1="Unbekannter Fehler Nr. "+String(iResult); break;
  }
 }
 else
 {
  switch(iResult)
  {
   case  -1: s1="EEPROM read: EEPROM not allowed"; break;
   case  -2: s1="EEPROM read: wrong begin"; break;
   case  -3: s1="EEPROM read: wrong length"; break;
   case  -4: s1="EEPROM read: read error"; break;
   case  -5: s1="EEPROM read: wrong end"; break;
   case -11: s1="EEPROM write: EEPROM not allowed"; break;
   case -12: s1="EEPROM write: sData too long"; break;
   case -13: s1="EEPROM write: eeprom size too small"; break;
   case -14: s1="EEPROM write: write error"; break;
   default:  s1="Unknown error #"+String(iResult); break;
  }
 }
 return s1;
}

//_______write topic to eeprom as topicBase_____________________
// Value in EEPROM: QQddxxxxx0UU0qqeeyyyyy0UU0
// dd=length of topicBase xxxxx, ee=length of string data yyyyy
// 0=VALUE 0 (not char!), Q,U are chars
// return: >=0: OK (length),
//         -11 EEPROM not allowed, -12 topic too long, 
//         -13 eeprom size too small, -14 write error
int SimpleMqtt::eepromWriteTopicBase(String topic)
{
 unsigned int iStart1=0;
 unsigned int len;
 int iRet=-11;
 if(!USE_EEPROM) return -11;
 //-----------does topic fit into eeprom?-----------------------
 len=topic.length();
 if(len>TOPIC_MAXLEN) return -12;
 if((len+iStart1)>(eepromSize_-8)) return -13;
 //-----------does topic + myData fit into eeprom?--------------
 int iResult2=0;
 String sMyData=eepromReadMyData(iResult2);
 if(iResult2==-1) return -11;          // EEPROM not allowed
 if(iResult2>=0)                       // >0 length of sMyData
 {
  if((len+iStart1+iResult2)>(eepromSize_-16)) return -13;
 }
 //-----------length OK, write topic + check values-------------
 char ca[len+8];
 ca[0]='Q';
 ca[1]='Q';
 ca[2]=(len/10)+'0';
 ca[3]=(len-10*(len/10))+'0';
 ca[4]=0;
 strcat(ca,topic.c_str());
 ca[len+4]=0;                          // value 0
 ca[len+5]='U';                        // U=0x55=0101 0101
 ca[len+6]='U';                        // U=0x55=0101 0101 
 ca[len+7]=0;                          // value 0
 iRet=eepromWriteBlock(ca,iStart1,len+8);// write to EEPROM
 //if(DEBUG_MQTT) Serial.println("eepromWriteTopicBase(): ca="+String(ca)+", ret="+String(ret));
 if(iRet<0) len=-14;                   // write error
 else {
  if(((unsigned int)iRet)!=(len+8)) len=-14; // write error
 }
 //-----------append myData (if exists ;) to topic--------------
 if(iResult2>=0) {
  eepromWriteMyData(sMyData);
 }
 return len;                           // OK: topic length
}

//_______read topic base from eeprom____________________________
// Value in EEPROM: QQddxxxxx0UU0qqeeyyyyy0UU0
// dd=length of topicBase xxxxx, ee=length of string data yyyyy
// 0=VALUE 0 (not char!), Q,U are chars
// return: read TopicBase ("" on error)
//         iResult >=0: OK (length),
//         -1 EEPROM not allowed, -2 wrong begin, 
//         -3 wrong length, -4 read error, -5 wrong end
String SimpleMqtt::eepromReadTopicBase(int& iResult)
{
 String s1="";
 if(USE_EEPROM) {
  char c4[5];
  unsigned int len1, len2;
  int len4=eepromReadBlock(c4,0,4);
  if(len4==4)
  {//---------wrong length of begin sequence--------------------
   if((c4[0]=='Q') && (c4[1]=='Q'))
   {//---------EEPROM starts with MQ-----------------------------
    len1=10*(c4[2]-'0')+(c4[3]-'0');
    if(len1<=(eepromSize_-4))
    {//--------stored topic length fits into eeprom--------------
     char ca[len1+4];
     len2=eepromReadBlock(ca,4,len1+4);
     if(len2==(len1+4))
     {//-------correct number of chars read----------------------
      if(ca[len1]==0 && ca[len1+1]=='U' && ca[len1+2]=='U' && ca[len1+3]==0)
      {//------tail of topic OK----------------------------------
       s1=String(ca);
       iResult=s1.length();
      }
      else { iResult=-5; }
     }
     else { iResult=-4; }
    }
    else { iResult=-3; }
   }
   else { iResult=-2; }
  }
  else { iResult=-2; }
 }
 else { iResult=-1; }
 //-----------show debug info-----------------------------------
 if(DEBUG_MQTT) {
  if(iResult>=0) Serial.println("eepromReadTopicBase(): "+s1+" - OK");
  else Serial.printf("eepromReadTopicBase(): failed! iResult=%d\n",iResult);
 }
 return s1;
}

//_______read topic from eeprom, ignore result__________________
// Value in EEPROM: MQddtopicBase0UU0 dd=length of topicBase,
//                  0=VALUE 0 (not char!)
// return: TopicBase (or "" on error)
String SimpleMqtt::eepromReadTopicBase()
{
 int iResult;
 String s1=eepromReadTopicBase(iResult);
 return s1;
}

//_______erase identifier of topicBase__________________________
// return true: erase ok, false: error occurred
bool SimpleMqtt::eepromEraseTopicBase()
{
 if(!USE_EEPROM) return false;
 char ca[5]={0,0,0,0,0};
 int iStart1=0;
 //-----read byData (directly after topicBase)------------------
 int iResult1, iResult2;
 String sMyData=eepromReadMyData(iResult2);
 //------erase topicBase----------------------------------------
 int ret=eepromWriteBlock(ca,iStart1,4);
 String s1=eepromReadTopicBase(iResult1);
 if(ret!=4 || iResult1>=0)
 {
  if(DEBUG_MQTT) Serial.println("eepromEraseTopicBase() Error"); 
  return false;                        // topicBase still exists
 }
 if(DEBUG_MQTT) Serial.println("eepromEraseTopicBase() OK"); 
 //-----topicBase erased: write sMyData (if exist)--------------
 if(iResult2>=0) eepromWriteMyData(sMyData);
 return true;
}

//_______write string data to eeprom (after topicBase)__________
// Value in EEPROM: QQddxxxxx0UU0qqeeyyyyy0UU0
// dd=length of topicBase xxxxx, ee=length of string data yyyyy
// 0=VALUE 0 (not char!), Q,U are chars
// return: >=0: OK (length),
//         -11 EEPROM not allowed, -12 sData too long, 
//         -13 eeprom size too small
int SimpleMqtt::eepromWriteMyData(String sData)
{
 unsigned int iStart2;
 unsigned int len;
 int iResult, iRet=-11;
 if(!USE_EEPROM) return -11;
 //-----------calculate start position in EEPROM----------------
 String sTop=eepromReadTopicBase(iResult);
 if(iResult==-1) return -11;           // EEPROM not allowed
 if(iResult<0) iStart2=0;              // no/wrong topic base
          else iStart2=sTop.length()+8;// next free position
 //-----------does data string fit into eeprom?-----------------
 len=sData.length();
 if(len>EEPROM_DATA_MAX) return -12;
 if((len+iStart2)>(eepromSize_-16)) return -13;
 //-----------length OK, write topic + check values-------------
//Serial.println("eepromWriteMyData(): sData="+sData);
 char ca[len+8];
 ca[0]='q';
 ca[1]='q';
 ca[2]=(len/10)+'0';
 ca[3]=(len-10*(len/10))+'0';
 ca[4]=0;
 strcat(ca,sData.c_str());
 ca[len+4]=0;                          // value 0
 ca[len+5]='U';                        // U=0x55=0101 0101
 ca[len+6]='U';                        // U=0x55=0101 0101 
 ca[len+7]=0;                          // value 0
 iRet=eepromWriteBlock(ca,iStart2,len+8);// write to EEPROM
 //if(DEBUG_MQTT) Serial.println("eepromWriteMyData(): ca="+String(ca)+", ret="+String(ret));
 if(iRet<0) len=-14;                   // write error
 else {
  if(((unsigned int)iRet)!=(len+8)) len=-14; // write error
 }
 return len;                           // OK: topic length
}

//_______read string data from eeprom___________________________
// Value in EEPROM: QQddxxxxx0UU0qqeeyyyyy0UU0
// dd=length of topicBase xxxxx, ee=length of string data yyyyy
// 0=VALUE 0 (not char!), Q,U are chars
// return: read string data ("" on error)
//         iResult >=0: OK (length),
//         -1 EEPROM not allowed, -2 wrong begin, 
//         -3 wrong length, -4 read error, -5 wrong end
String SimpleMqtt::eepromReadMyData(int& iResult)
{
 String s1=String("");
 int iStart2;
 unsigned int len1, len2;
 char c4[5];
 if(USE_EEPROM) {
  //----------calculate start position in EEPROM----------------
  String sTop=eepromReadTopicBase(iResult);
  if(iResult==-1) return s1;
   if(iResult<0) iStart2=0;              // no/wrong topic base
            else iStart2=sTop.length()+8;// next free position
  int len4=eepromReadBlock(c4,iStart2,4);
  if(len4==4)
  {//---------wrong length of begin sequence--------------------
   if((c4[0]=='q') && (c4[1]=='q'))
   {//--------EEPROM starts with MQ-----------------------------
    len1=10*(c4[2]-'0')+(c4[3]-'0');
    if(len1<=(eepromSize_-4))
    {//-------stored string data length fits into eeprom--------
     char ca[len1+4];
     len2=eepromReadBlock(ca,iStart2+4,len1+4);
     if(len2==(len1+4))
     {//-------correct number of chars read----------------------
      if(ca[len1]==0 && ca[len1+1]=='U' && ca[len1+2]=='U' && ca[len1+3]==0)
      {//------tail of topic OK----------------------------------
       s1=String(ca);
       iResult=s1.length();
      }
      else { iResult=-5; }
     }
     else { iResult=-4; }
    }
    else { iResult=-3; }
   }
   else { iResult=-2; }
  }
  else { iResult=-2; }
 }
 else { iResult=-1; }
 //-----------show debug info-----------------------------------
 if(DEBUG_MQTT) {
  if(iResult>=0) Serial.println("eepromReadTopicBase(): "+s1+" - OK");
  else Serial.printf("eepromReadTopicBase(): failed! iResult=%d\n",iResult);
 }
 return s1;
}

//_______read string data from eeprom, ignore result____________
String SimpleMqtt::eepromReadMyData()
{
 int iResult=0;
 String s1=eepromReadMyData(iResult);
 return s1;
}

//_______erase string data from eeprom__________________________
bool SimpleMqtt::eepromEraseMyData()
{
 if(!USE_EEPROM) return false;
 int iResult, iStart2;
 char ca[5]={0,0,0,0,0};
 //-----------calculate start position in EEPROM---------------
 String sTop=eepromReadTopicBase(iResult);
 if(iResult<0) iStart2=0;
          else iStart2=iResult+8;
 //-----------erase block---------------------------------------
 int ret=eepromWriteBlock(ca,iStart2,4);
 if(ret==4) {
  String s1=eepromReadMyData(iResult);
  if(iResult<0) {
   if(DEBUG_MQTT) Serial.println("eepromEraseMyData() OK"); 
   return true;
  }
 }
 if(DEBUG_MQTT) Serial.println("eepromEraseMyData() Error"); 
 return false;
}

//_______read a block from eeprom_______________________________
// uses: eepromSize_
// return: number of read bytes or -1|-2|-3|-4 on error
size_t SimpleMqtt::eepromReadBlock(char* data, 
       unsigned long address, unsigned long len)
{
 unsigned long i;
 //------check input--------------------------------------------
 if(len==0)    return -1;
 //if(len<0)     return -2;
 //if(address<0) return -3;
   if((address+len)>eepromSize_) len=eepromSize_-address;
 //------read bytes---------------------------------------------
 if(DEBUG_MQTT) Serial.print("eepromReadBlock(): ");
 eeprom_->begin(eepromSize_);
 for(i=0; i<len; i++) {
  data[i]=(char)(eeprom_->read(address+i));
  if(DEBUG_MQTT) Serial.printf("%ld=%c=%d|",(address+i),data[i],data[i]);
 }
 eeprom_->end();
 if(DEBUG_MQTT) Serial.printf("=> %ld bytes read.\n",len);
 return len;
}

//_______write a block to eeprom________________________________
// uses: eepromSize_
// return: number of written bytes or -1|-2|-3|-4 on error
size_t SimpleMqtt::eepromWriteBlock(char* data, 
       unsigned long address, unsigned long len)
{
 unsigned long i=0, numBytes=0;
 uint8_t b1, b2;
 //------check input--------------------------------------------
 if(len==0)    return -1;
 //if(len<0)     return -2;
 //if(address<0) return -3;
 if((address+len)>eepromSize_) return -4;
 //------write bytes--------------------------------------------
 if(DEBUG_MQTT) Serial.print("eepromWriteBlock(): ");
 eeprom_->begin(eepromSize_);
 //......write all bytes........................................
 for(i=0; i<len; i++)
 {
  b1=data[i];
  eeprom_->write(address+i, b1);
  eeprom_->commit();
  //delay(1);
  unsigned long _millisStart_=millis();
  while(millis()-_millisStart_<1) yield(); // wait 1ms
  b2=eeprom_->read(address+i);
  if(b1==b2) numBytes++;
  if(DEBUG_MQTT) {Serial.printf("%ld=%c|",(address+i),b2); }
 }
 eeprom_->end();
 if(len==numBytes)
 {
  if(DEBUG_MQTT) Serial.println(" ok");
  return numBytes;
 }
 if(DEBUG_MQTT) Serial.println(" ERROR");
 return -2;
}
