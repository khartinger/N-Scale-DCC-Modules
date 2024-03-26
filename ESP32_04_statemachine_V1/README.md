<a href="./LIESMICH.md">==> Deutsche Version</a>   
Last update: March 26, 2024 <a name="up"></a>   
<h1>ESP32: state machine</h1>   

# Target
This program for an ESP32 (or D1mini) uses a state machine to generate a flashing light on the built-in LED. In addition, the status of the LED and the duration of the status are displayed via the serial interface.    

# Required hardware
1. ESP32 D1 mini or WeMos D1 mini   

# Quick guide
1. compile program `ESP32_04_statemachine_V1.cpp` and upload it to the ESP32 D1 mini.   
2. the ESP32 D1 mini's built-in blue LED flashes every second.   

# Program details
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

[To the top of the page](#up)
