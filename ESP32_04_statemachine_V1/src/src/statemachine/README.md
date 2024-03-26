# D1 mini: class `Statemachine` and `StatemachineBlink`
Version 2020-02-25, Files: D1_class_Statemachine.h + D1_class_Statemachine.cpp and  D1_class_StatemachineBlink.h + D1_class_StatemachineBlink.cpp   
D1_oop38_statemachineBlink.ino   
[German version](./LIESMICH.md "Deutsche Version")   

## Class Statemachine
The class `Statemachine` implements a counter, that counts endless from a first to a second value. In every step it waits the given time.   
Default values are: Count from 1 to 10, wait every step for 200 ms. Result: The whole cycle time is 10*200ms = 2 seconds.   
Furthermore, the time is measured, that the commands within the step need.   

### Constructors for class Statemachine
There are three constructors available:
1. `Statemachine();` ... Use the default values (count from 1 to 10, duration of one step 200ms)   
2. `Statemachine(int state_max, int state_delay);` ... Count from 1 to the given value state_max, duration of one step is delay ms   
3. `Statemachine(int state_min, int state_max, int state_delay);` ... Count from state_min to state_max, duration of one step is delay ms   

### Important methods
* `loopBegin();` ... __Must be called__ at the beginning of loop() - returns the state number.   
* `loopEnd();` ... __Must be called__ at the end of loop() - returns the duration of the commands between loopBegin() and loopEnd()   
* `add(states);` ... Add "states" to the actual state number. Takes a possible overflow into account.   

### Example for class Statemachine
The following example counts from 1 to 10 and waits for 500 ms every step. The state number and the duration of command executions in this step are printed to Serial (115200Bd).
 
_Assumption_: The files D1_class_statemachine.h and D1_class_statemachine.cpp are located in directory src/statemachine.   
```   
//_____D1_test_statemachine1.ino_________________2020-02-26_____
#include "src/statemachine/D1_class_Statemachine.h"
Statemachine statemachine(10,500);
void setup() {
 Serial.begin(115200); Serial.println("\nStart");
}
void loop() {
 int state=statemachine.loopBegin();        // state begin 
 int duration=statemachine.loopEnd();       // state end
 Serial.println("state="+String(state)+", duration="+String(duration)+"ms");
}
```   
Result:
```   
Start
state=1, duration=0ms
state=2, duration=0ms
state=3, duration=0ms
state=4, duration=0ms
state=5, duration=0ms
state=6, duration=0ms
state=7, duration=0ms
state=8, duration=0ms
state=9, duration=0ms
state=10, duration=0ms
state=1, duration=0ms
state=2, duration=0ms
...
```   
As can be seen,   
* no time is needed between begin and end of the loop. (Because there are no commands there...),   
* lines are printed to Serial every 0.5 seconds,   
* after step 10 follows step 1 and so on.   
--- 

## Class StatemachineBlink
The class `StatemachineBlink` can be used to realize easily blinking leds within a state machine.  
To build a StatemachineBlink object, you need to know...   
* the pin number, where the LED is wired (e.g. D4)
* LED is on, if pin voltage is 3V3 or 0V (= inverted)
* the state number where the LED starts blinking the first time
* the duration for LED is on (in states)
* the duration for LED is off (in states)
* the number of blinks (-1 = endless)

### Constructors for class StatemachineBlink
There are two constructors available:
1. `StatemachineBlink();` ... Use the default values   
2. `StatemachineBlink(LED1_PIN, LED1_INVERT, STATE_LED1_START, STATES_LED1_ON, STATES_LED1_OFF, LED1_BLINK_NUM);` ... Set the values from above   

__*Examples:*__   
_Assumption_: The duration of one state of the statemachine is 0.1 sec   
* `StatemachineBlink led1(D4,true,1,5,5,3);` ... Blue LED blinks on every statemachine cycle start 3x at one-second intervals
* `StatemachineBlink led1(D4,true,20,1,9,5);` ... Blue LED flashes two seconds after statemachine cycle start 5 times
* `StatemachineBlink led7(D7,false,1,5,15,-1);` and   
`StatemachineBlink led8(D8,false,6,15,5,-1);` ... LEDs at D7 and D8 build an endless flashing light. LED D8 lights 3 times longer than D7   

### Important methods
The method `doBlink(statemachine)` needs a Statemachine object and controls the state of the LED.

### Example for class StatemachineBlink
The following example lets the blue LED flush every 3 seconds.   
Rrealisation:
1. The state machine counts from 1 to 30 with 100ms delay per step
2. The LED is switched on once at state 5 for 2 states. Don't care about the duration of off-condition (value set to 0).   

_Assumption_: The files D1_class_statemachine.* and D1_class_statemachineBlink.* are located in directory src/statemachine. 
```
//_____D1_test_statemachine2.ino_________________2020-02-26_____
#include "src/statemachine/D1_class_Statemachine.h"
#include "src/statemachine/D1_class_StatemachineBlink.h"
Statemachine statemachine(30,100);
StatemachineBlink led1(D4,true,3,2,0,1);
void setup() {}
void loop() {
 statemachine.loopBegin();                  // state begin
 led1.doBlink(statemachine);                // blink LED
 statemachine.loopEnd();                    // state end
}
```
