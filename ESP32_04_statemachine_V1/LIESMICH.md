<a href="./README.md">==> English version</a>   
Letzte &Auml;nderung: 26.3.2024 <a name="up"></a>   
<h1>ESP32: Zustandsz&auml;hler (state machine)</h1>   

# Ziel
Dieses Programm f&uuml;r einen ESP32 (oder D1mini) erzeugt mit Hilfe eines Zustandsz&auml;hlers (state machine) ein Blinklicht an der eingebauten LED. Weiters wird &uuml;ber die serielle Schnittstelle der Zustand der LED  sowie die Dauer des Zustandes angezeigt.   

# Erforderliche Hardware
1. ESP32 D1 mini oder WeMos D1 mini   

# Kurzanleitung
1. Programm `ESP32_04_statemachine_V1.cpp` compilieren und auf den ESP32 D1 mini hochladen.   
2. Die am ESP32 D1 mini verbaute, blaue LED blinkt im Sekundentakt.   

# Programmdetails
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

[Zum Seitenanfang](#up)
