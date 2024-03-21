<a href="./README.md">==> English version</a>   
Letzte &Auml;nderung: 21.3.2024 <a name="up"></a>   
<h1>ESP32: MQTT Grundfunktionen</h1>   

# Ziel
Dieses Programm für einen ESP32 (oder D1mini) dient zum Empfangen und Senden von MQTT-Nachrichten und stellt gewisse [Basisbefehle](#a10) zur Verfügung.   

# Erforderliche Hardware
1. ESP32 D1 mini oder WeMos D1 mini   
2. PC oder Laptop zum Senden und Empfangen von MQTT-Nachrichten.   

# Kurzanleitung
1. Programm `ESP32_03_smqtt_V1.cpp.cpp` compilieren und auf den ESP32 D1 mini hochladen.   
2. Die ESP32 D1 mini eingebaute, blaue LED blinkt im Sekundentakt und zeigt damit an, dass versucht wird, eine Verbindung zum MQTT-Server herzustellen.   
3. Kann die Verbindung erfolgreich hergestellt werden, leuchtet die blaue LED konmtinuierlich. Verlischt die LED, konnte keine Verbindung zum MQTT-Server hergestellt werden.   
   
Die Ausführung der Basisbefehle wird im Kapitel ["MQTT Basisbefehle"](#a10) beschrieben.   

# Programmdetails
## Netzwerk- und MQTT-Daten
Die Netzwerk- und MQTT-Daten werden in einer eigenen Konfigurationsdatei gespeichert und könnten zB folgendermaßen definiert sein:   
```   
//_______Network and MQTT data__________________________________
 #define  _USE_WIFI_    true
 #define  _SSID_        "Raspi11"
 #define  _PASS_        "12345678"
 #define  _HOST_        "10.1.1.1"
 #define  TOPIC_BASE    "modul1"
 #define  TOPIC_GET     "?,help,version,ip,topicbase,eeprom"
 #define  TOPIC_SET     "topicbase,eeprom0"
 #define  TOPIC_SUB     ""
 #define  TOPIC_PUB     ""
```   

Darin bedeutet ...   
| Konstante  | Zweck                                                        |   
|------------|--------------------------------------------------------------|   
| `_USE_WIFI_` | true: Es soll eine Steuerung über Netzwerk möglich sein.     |   
| `_SSID_`     | Name des Netzwerks, in das eingewählt werden soll.           |   
| `_PASS_`     | Passwort des Netzwerks, in das eingewählt werden soll.       |   
| `_HOST_`     | Name des Servers (oder IP-Adresse), bei dem eingewählt werden soll.  |   
| `TOPIC_BASE` | Basis Topic.  |   
| `TOPIC_GET`  | Alle möglichen get-Anfragen (durch Beistriche getrennt).  |   
| `TOPIC_SET`  | Alle möglichen get-Anfragen (durch Beistriche getrennt).  |   
| `TOPIC_SUB`  | Topics, die ebenfalls empfangen werden sollen (ohne Berücksichtigung des Basis-Topics).  |   
| `TOPIC_PUB`  | Topics unter denen publiziert werden soll (ohne Berücksichtigung des Basis-Topics).  |   

Beispiele siehe Kapitel ["MQTT Basisbefehle"](#a10).   

## Anzeige auf serieller Schnittstelle
Ist der Debug-Modus eingeschaltet (`#define DEBUG_03 true`) wird im seriellen Monitor der Zustand der Netzwerkverbindung angezeigt, wie zum Beispiel   
```   
setup(): --Start--
setup(): WiFi Raspi11 connecting...
........
setup(): --Finished--

 | MQTT OK
 | -No WiFi--
 | MQTT OK
```   

## MQTT Kommunikation   
Die Herstellung der WLAN-Verbindung zum MQTT-Server erfolgt mit der Klasse `SimpleMqtt`, die auf der Klasse `PubSubClient` aufbaut.   

<a name="a10"></a>   

## MQTT Get-Basisbefehle   
Get-Befehle dienen zur Abfrage von Werten oder Hardware-Zuständen. Was abgefragt werden soll steht in der Nachricht.   
Get-Befehle setzen sich aus einem Basis-Topic und einem angehängten `/get` zusammen. Als Antwort erhält man eine Nachricht, deren Topic sich aus dem Basis-Topic und `/ret` zusammensetzt.   

### Beispiel: Abfrage der Programmversion   
Annahme 1: Das Senden der MQTT-Nachricht erfolgt mit dem Programm `mosquitto_pub`.   
Annahme 2: Die IP-Addresse des Host-Rechners ist `10.1.1.1`   
Annahme 3: Das Basis-Topic lautet `modul1`   
   
MQTT-Befehl:   
`mosquitto_pub -h 10.1.1.1 -t modul1/get -m version`   

Um die Antwort des Servers zu sehen, muss in einem zweiten Konsolenfenster das Programm `mosquitto_sub` laufen:   
`mosquitto_sub -h 10.1.1.1 -t "#" -v`   
Ist dies der Fall, erhält man folgende Ausgabe:   
```   
modul1/get version
modul1/ret/version {"version":"2024-03-21 ESP32_03_smqtt_V1"}
```   

### Weitere Get-Befehle   
* Abfrage der Hilfe   
   `mosquitto_pub -h 10.1.1.1 -t modul1/get -m ?`   
   `mosquitto_pub -h 10.1.1.1 -t modul1/get -m help`   
* Abfrage der IP-Adresse   
   `mosquitto_pub -h 10.1.1.1 -t modul1/get -m ip`   
* Abfrage des Basis-Topics   
   `mosquitto_pub -h 10.1.1.1 -t modul1/get -m topicbase`   
* Abfrage des EEPROM-Inhalts   
   `mosquitto_pub -h 10.1.1.1 -t modul1/get -m eeprom`   

## MQTT Set-Basisbefehle   
Set-Befehle dienen zum Setzen von Werten oder zum Ansteuern der Hardware. Sie setzen sich aus dem Basis-Topic und einem angehängten `/set/xxx` zusammen, wobei `xxx` für den Wert steht, der geändert werden soll. Auf welchen Wert `xxx` geändert werden soll, steht in der Nachricht. Als Antwort erhält man eine Nachricht, deren Topic sich aus dem Basis-Topic und `/ret` zusammensetzt.   

### Setzen des Basis-Topics
Das Setzen eines neuen Basis-Topics ändert sofort den Wert des Basis-Topics. Weiters wird der Wert im EEPROM des ESP32 abgelegt und bei jedem Einschalten von dort geholt.   
Beachte 1. Das Basis-Topic enthält nur Kleinbuchstaben!   
Beachte 2. Wird das Basis-Topic geändert (und somit im EEPROM gespeichert), so bleibt es geändert, auch wenn man den ESP32 neu programmiert!   
Beim Start des ESP wird eine Nachricht mit dem Topic `info/start/mqtt` gesendet, das das verwendete Basis-Topic enthält:   
`info/start/mqtt {"topicbase":"modul1","IP":"10.1.1.185"}`   

__*Beispiel*__: Setzen des Basis-Topics auf `Modul1xxx`   
`mosquitto_pub -h 10.1.1.1 -t modul1/set/topicbase -m Modul1xxx`   
Antwort:   
`modul1a/ret/topicbase {"topicbase":"modul1xxx"}`   
Kontrolle durch Überprüfen des EEPROM-Inhalts:   
`mosquitto_pub -h 10.1.1.1 -t modul1xxx/get -m eeprom`   
Antwort:   
`modul1xxx/ret/eeprom {"topicbase":"modul1xxx","mydata":"(no data)"}`   

### Löschen des EEPROM-Inhalts
Das Löschen des EEPROM-Inhalts erfolgt durch den Befehl   
`mosquitto_pub -h 10.1.1.1 -t modul1/set/eeprom0 -m ?`   
wobei als Nachricht der Wert 1, 2 oder 3 gesetzt wird, je nachdem, was gelöscht werden soll:   
1 = Basis-Topic, 2 = myData, 3 = alles   

[Zum Seitenanfang](#up)
