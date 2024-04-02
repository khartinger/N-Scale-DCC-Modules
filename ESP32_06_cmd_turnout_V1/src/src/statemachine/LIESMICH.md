# D1 mini: Klasse `Statemachine` und `StatemachineBlink`
Version 2020-02-25, Dateien: D1_class_Statemachine.h + D1_class_Statemachine.cpp bzw. D1_class_StatemachineBlink.h + D1_class_StatemachineBlink.cpp   
[English version](./README.md "English version")   

## Klasse Statemachine
Die Klasse `Statemachine` realisiert einen Z&auml;hler, der endlos von einer ersten bis zu einer zweiten Zahl z&auml;hlt und nach jedem Schritt eine vorgegebene Zeit wartet.   
Vorgabewerte sind: Z&auml;hlen von 1 bis 10, Dauer eines Schrittes 200ms, dh. die Zykluszeit betr&auml;gt 10*200ms = 2 Sekunden.   
Weiters wird die Zeit bestimmt, die die Ausf&uuml;hrung der Befehle innerhalb des Schrittes gedauert hat.

### Konstruktoren f&uuml;r die Statemachine
Es sind drei Konstruktoren vorhanden:
1. `Statemachine();` ... Verwendet die Vorgabewerte (Z&auml;hlen von 1 bis 10, Dauer eines Schrittes 200ms)   
2. `Statemachine(int state_max, int state_delay);` ... Z&auml;hlt von 1 bis zum angegebenen Wert state_max, Dauer eines Schrittes ist delay ms   
3. `Statemachine(int state_min, int state_max, int state_delay);` ... Z&auml;hlt von state_min bis zum angegebenen Wert state_max, Dauer eines Schrittes ist delay ms   

### Wichtige Methoden
* `loopBegin();` ... __Muss__ am Beginn der loop()-Funktion ausgef&uuml;hrt werden, liefert die State-Nummer zur&uuml;ck.   
* `loopEnd();` ... __Muss__ am Ende der loop()-Funktion ausgef&uuml;hrt werden. Liefert die Zeitdauer f&uuml;r die Bearbeitung der Befehle, die zwischen loopBegin() und loopEnd() stehen.   
* `add(states);` ... Addieren von "states" zum aktuellen State. Ber&uuml;cksichtigt einen eventuellen &Uuml;berlauf.   


### Anwendungsbeispiel f&uuml;r die Klasse Statemachine
Das folgende Beispiel z&auml;hlt von 1 bis 10 und wartet 500 ms pro Schritt. Die Statenummer und  die ben&ouml;tigte Zeit f&uuml;r die Befehle des Schrittes werden an die   die serielle Schnittstelle gesendet (115200Bd).
Annahme: Die D1_class_statemachine.* Dateien befinden sich im Ordner src/statemachine.
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
Ergebnis:
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
Man erkennt, dass   
* zwischen dem Beginn und dem Ende des Loops (praktisch) keine Zeit ben&ouml;tigt wird, da keine Befehle vorhanden sind,   
* trotzdem zwischen den einzelnen Ausgaben 0,5 Sekunden vergehen.
* nach dem Schritt 10 wieder der Schritt 1 folgt usw.   
--- 

## Klasse StatemachineBlink
Die Klasse `StatemachineBlink` dient dazu, in einer Statemachine auf einfache Weise ein Blinklicht (oder mehrere) zu realisieren.   

Zum Erstellen des Blinkobjekts ben&ouml;tigt man folgende Informationen:   
* Die Nummer des Pins, an dem die LED angeschlossen ist (zB D4)   
* Leuchtet die LED, wenn der Pin auf 3V3 liegt (invertiert=false) oder bei 0V (invertiert=true)?   
* Die State-Nummer, bei der die LED zum ersten Mal zu leuchten beginnen soll   
* die Dauer des LED-ein-Zustandes (in States)   
* die Dauer des LED-aus-Zustandes (in States)   
* die Anzahl der Blinker (-1 bedeutet Dauerblinken)

### Konstruktoren f&uuml;r das Blinklicht
1. `StatemachineBlink();` ... Nutzt die Vorgabewerte   
2. `StatemachineBlink(LED1_PIN, LED1_INVERT, STATE_LED1_START, STATES_LED1_ON, STATES_LED1_OFF, LED1_BLINK_NUM);` ... Setzt die oben beschriebenen Werte   

__*Beispiele:*__   
_Annahme_: Die Dauer eines States der Statemachine ist 0,1 sec   
* `StatemachineBlink led1(D4,true,1,5,5,3);` ... Die blaue LED blinkt bei jedem Statemachine-Zyklusstart 3x im Sekundentakt
* `StatemachineBlink led1(D4,true,20,1,9,5);` ... Die blaue LED blitzt zwei Sekunden nach Beginn des Statemachine-Zyklusstarts 5x kurz auf
* `StatemachineBlink led7(D7,false,1,5,15,-1);` und   
`StatemachineBlink led8(D8,false,6,15,5,-1);` ... Die LEDs an D7 und D8 bilden ein Endlos-Wechselblinklicht, wobei die LED D8 dreimal so lange eingeschaltet ist wie D7

### Wichtige Methoden
Die Methode `doBlink(statemachine)` ben&ouml;tigt als Argument das Statemachine-Objekt und steuert so den Zustand der LED.

### Anwendungsbeispiel f&uuml;r die Klasse StatemachineBlink
Das folgende Programmbeispiel erzeugt alle 3 Sekunden einen kurzes Aufblitzen der blauen LED.   
Realisierung:
1. Die Statemachine z&auml;hlt von 1 bis 30 mit 100ms Verz&ouml;gerung pro Schritt
2. Die LED wird beim Z&auml;hlerstand 5 f&uuml;r 2 Takte einmal eingeschaltet. Die Zeit f&uuml;r den Aus-Zustand ist egal (hier beispielsweise auf 0 gesetzt).   

Annahme: Die Dateien D1_class_statemachine.* und D1_class_statemachineBlink.* befinden sich im Ordner src/statemachine.
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
