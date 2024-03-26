//_____D1_class_StatemachineBlink.cpp_________200224-200306_____
// The class StatemachineBlink helps to blink a LED in a state 
// machine. You need to know...
// ... the pin number, where the LED is wired (e.g. D4)
// ... LED is on, if pin voltage is 3V3 or 0V (= inverted)
// ... the state number when LED should start blinking
// ... the duration for LED is on (in states)
// ... the duration for LED is off (in states)
// ... the number of blinks (-1 = endless)
// if statesOn_ =0, the blue LED is turned off continuously
// if statesOff_=0, the blue LED is turned on  continuously
// Created by Karl Hartinger, February 24, 2020.
// Modified 2020-02-28 setStateStart() added
//          2020-03-03 goOn() added
//          2020-03-06 on() added
// Released into the public domain.

#include "D1_class_StatemachineBlink.h"

//**************************************************************
//     constructor & co
//**************************************************************

//_____constructor 1_____________________________________________
StatemachineBlink::StatemachineBlink() { 
 setup();
 pinMode(pin_, OUTPUT);                // led pin = output
 setLed(ledIsOn_);
}

//_____constructor 2_____________________________________________
StatemachineBlink::StatemachineBlink(
 int pin, bool invert,
 int stateStart, int statesOn, int statesOff, int blinkNum)
{
 setup();
 pin_=pin;                             // LED pin number
 invert_=invert;                       // LED inverted?
 stateStart_=stateStart;
 statesOn_  =statesOn;
 statesOff_ =statesOff;
 blinkMax_  =blinkNum;
 pinMode(pin_, OUTPUT);                // led pin = output
 setLed(ledIsOn_);
} 

//_____setup properties_________________________________________
void StatemachineBlink::setup() {
 pin_       =SMB_PIN;                  // LED pin number
 invert_    =SMB_INVERT;               // LED inverted?
 stateStart_=SMB_STATE_START;
 statesOn_  =SMB_STATES_ON;
 statesOff_ =SMB_STATES_OFF;
 blinkMax_  =SMB_BLINK_ENDLESS;
 stateOff_  =SMB_STATE_INVALID;
 restart_   =true;
 started_   =false;
 off_       =false;
 ledIsOn_   =false;
 ledBeforeOff_=false;
}

//_____turn led logical on (true) or off (false)________________
// return: logical state of led (true=on, false=off)
bool StatemachineBlink::setLed(bool turnledon)
{
 if(turnledon)
 {
  if(invert_) digitalWrite(pin_, LOW); // led on
         else digitalWrite(pin_, HIGH);// led on
  ledIsOn_=true;
 }
 else 
 {
  if(invert_) digitalWrite(pin_, HIGH);// led off
         else digitalWrite(pin_, LOW); // led off
  ledIsOn_=false;
 }
 return ledIsOn_;
}

//**************************************************************
//     set/get values
//**************************************************************
//_____set new start state and wait for start state number______
void StatemachineBlink::setStateStart(int stateStart)
{
 stateStart_=stateStart;
 restart_   =true;
 started_   =false;
 off_       =false;
 stateOff_  =SMB_STATE_INVALID;
}

//_____restart blink function___________________________________
void StatemachineBlink::restart(int stateStart)
{ setStateStart(stateStart); }

//_____set new blink parameter and wait for start state number__
void StatemachineBlink::setParams(int stateStart, int statesOn,
 int statesOff, int blinkNum) {
  statesOn_  =statesOn;
  statesOff_ =statesOff;
  blinkMax_  =blinkNum;
  setStateStart(stateStart);
 }

//**************************************************************
//     working methods
//**************************************************************
//_____check state number, if something is to do________________
// returns the number following blinks
int StatemachineBlink::doBlink(Statemachine stm)
{
 if(off_) return 0;
 if(statesOn_ ==0) { setLed(false); return 0; }
 if(statesOff_==0) { setLed(true); return 0; }
 int state=stm.getState();
 if(SMB_DEBUG) { Serial.print("SMB doBlink: state=");Serial.print(state);}
 //-----start blinking?-----------------------------------------
 if(state==stateStart_)
 {
  if(restart_)
  {
   if(SMB_DEBUG){Serial.print(", pin=");Serial.print(pin_);Serial.print(" start blinking");}
   setLed(true);                       // led on
   blinkCounter_=blinkMax_;
   nextStateOff_=stm.add(statesOn_);
   if(blinkMax_==SMB_BLINK_ENDLESS) restart_=false;
   started_=true;
  }
 }
 if(!started_) return blinkCounter_;
 //-----turn led off?-------------------------------------------
 if(state==nextStateOff_)
 {
  if(SMB_DEBUG){Serial.print(", pin=");Serial.print(pin_);Serial.print(" led off");}
  setLed(false);                       // led off
  if(blinkCounter_>0) blinkCounter_--;
  if((blinkCounter_>0)||(blinkCounter_==SMB_BLINK_ENDLESS))
  nextStateOn_=stm.add(statesOff_);
 }
 //-----turn led on?--------------------------------------------
 if(state==nextStateOn_)
 {
  if(SMB_DEBUG){Serial.print(", pin=");Serial.print(pin_);Serial.print(" led on");}
  setLed(true);                        // led on
  nextStateOff_=stm.add(statesOn_);
 }
 if(SMB_DEBUG) Serial.println();
 return blinkCounter_;
}

//_____turn blinking off________________________________________
void StatemachineBlink::off(Statemachine stm) {
 stateOff_=stm.getState();
 off_=true; 
 ledBeforeOff_=ledIsOn_;
 setLed(false);                        // led off
}

//_____turn blinking on_________________________________________
void StatemachineBlink::on(Statemachine stm) {
 stateOff_=stm.getState();
 off_=true; 
 ledBeforeOff_=ledIsOn_;
 setLed(true);                         // led on
}

//_____allow blinking again_____________________________________
// return: logical state of led (true=on, false=off)
bool StatemachineBlink::goOn(Statemachine stm)  { 
 if(!off_) return false;
 int statesDiff=stm.diff(stateOff_);
 nextStateOn_=stm.add(nextStateOn_, statesDiff);
 nextStateOff_=stm.add(nextStateOff_, statesDiff);
 off_=false;
 setLed(ledBeforeOff_);
 return ledBeforeOff_;
}
