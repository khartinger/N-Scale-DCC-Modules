//_____D1_class_Statemachine.h________________181002-240220_____
// The class Statemachine helps to build a state counter.
// It counts from 1 to stateMax (incl.) and waits
// "stateDelay" milliseconds for every state.
// Created by Karl Hartinger, October 02, 2018.
// Modified
// 2018-10-05 some set/get added
// 2018-10-19 stateMin, add() added
// 2020-01-19 getBeginMillis() added
// 2020-02-25 getStateMin() added
// 2020-03-03 add(..,..), diff() added
// 2020-04-05 add getBeginMillis()
// 2021-01-31 state: change int to int32_t
// 2021-04-11 add isDelayed(), delayed (ms)
// 2024-02-20 replace delay() by while(), add getDelayed()
// 2024-02-22 defines STATE_MINIMUM, STATE_NOW, STATE_NONE
// Released into the public domain.
#ifndef D1_CLASS_STATEMACHINE_H
#define D1_CLASS_STATEMACHINE_H
#include "Arduino.h"
#define STATE_ONE                1     // 1st value of state
#define STATE_LAST              10     // last value of states
#define STATE_DELAY_DEFAULT    200     // state delay in ms
#define STATE_MINIMUM  -2147483644
#define STATE_NOW      -2147483645
#define STATE_NONE     -2147483646

class Statemachine {
 protected:
  //-----properties---------------------------------------------
  int32_t  stateCounter;               // counts from min to max
  int32_t  stateMin;                   // 1st counter value
  int32_t  stateMax;                   // last vounter value
  uint32_t stateDelay;                 // delay for one state
  uint32_t millisBegin;                // start time of state
  uint32_t delayed;                    // state(s) delayed in ms
 public:
  //-----constructor & co---------------------------------------
  Statemachine();                      // use default values
  Statemachine(int32_t state_max, int32_t state_delay);
  Statemachine(int32_t state_min, int32_t state_max, int32_t state_delay);
 protected:
  void setup();                        // setup properties
 public:
  //-----set values---------------------------------------------
  bool setStateMin(int32_t state_min); //
  bool setStateMax(int32_t state_max); //
  bool setStateDelay(int32_t state_delay); //
  bool setState(int32_t new_state);    //
  //-----get values---------------------------------------------
  int32_t  getStateMin();              //
  int32_t  getStateMax();              //
  int32_t  getStateDelay();            //
  int32_t  getState();                 //
  int32_t  getDuration();              //
  uint32_t getDelayed();               // state(s) delayed in ms
  uint32_t getBeginMillis();           //
  //-----working methods----------------------------------------
  int32_t  loopBegin();                //
  uint32_t loopEnd();                  //
  int32_t  add(int32_t numberOfStates);// return(stateCounter+num)
  int32_t  add(int32_t state, int32_t numberOfStates); // return(state+num)
  int32_t  diff(int32_t oldState);     // return(stateCounter-old)
  bool     isDelayed();                //
};
#endif
