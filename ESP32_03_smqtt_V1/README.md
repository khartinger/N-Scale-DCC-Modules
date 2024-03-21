<a href="./LIESMICH.md">==> Deutsche Version</a>   
Last update: March 21, 2024 <a name="up"></a>   
<h1>ESP32: MQTT basic functions</h1>   

# Target
This program for an ESP32 (or D1mini) is used to receive and send MQTT messages and provides certain [basic commands](#a10).   

# Required hardware
1. ESP32 D1 mini or WeMos D1 mini   
2. PC or laptop for sending and receiving MQTT messages.   

# Quick guide
1. compile program `ESP32_03_smqtt_V1.cpp.cpp` and upload it to the ESP32 D1 mini.   
2. the ESP32 D1 mini's built-in blue LED flashes every second to indicate that an attempt is being made to establish a connection to the MQTT server.   
3. if the connection can be established successfully, the blue LED lights up continuously. If the LED goes out, no connection to the MQTT server could be established.   
   
The execution of the basic commands is described in chapter ["MQTT basic commands"](#a10).   

# Program details
## Network and MQTT data
The network and MQTT data are saved in a separate configuration file and could be defined as follows, for example:   
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

In it means ...   
| Constant | Purpose |   
|------------|--------------------------------------------------------------|   
| `_USE_WIFI_` | true: Control via network should be possible.     |   
| `_SSID_` | Name of the network to be dialed into.           |   
| `_PASS_` | Password of the network to be dialed into.       |   
| `_HOST_` | Name of the server (or IP address) to be dialed into.  |   
| `TOPIC_BASE` | Base topic.  |   
| `TOPIC_GET` | All possible get requests (separated by commas).  |   
| `TOPIC_SET` | All possible get requests (separated by commas).  |   
| `TOPIC_SUB` | Topics that are also to be received (without taking the base topic into account).  |   
| `TOPIC_PUB` | Topics to be published under (without taking the base topic into account).  |   

For examples, see chapter ["MQTT basic commands"](#a10).   

## Display on serial interface
If the debug mode is switched on (`#define DEBUG_03 true`), the status of the network connection is displayed in the serial monitor, for example   
```   
setup(): --Start--
setup(): WiFi Raspi11 connecting...
........
setup(): --Finished--

 | MQTT OK
 | -No WiFi--
 | MQTT OK
```   

## MQTT communication
The WLAN connection to the MQTT server is established using the 'SimpleMqtt' class, which is based on the 'PubSubClient' class.

<a name="a10"></a>   

## MQTT Get basic commands   
Get commands are used to query values or hardware states. What is to be queried is specified in the message.   
Get commands consist of a basic topic and an appended `/get`. The response is a message whose topic is made up of the base topic and `/ret`.   

### Example: Querying the program version   
Assumption 1: The MQTT message is sent with the program `mosquitto_pub`.   
Assumption 2: The IP address of the host computer is `10.1.1.1`.   
Assumption 3: The base topic is `module1`.   
   
MQTT command:   
`mosquitto_pub -h 10.1.1.1 -t modul1/get -m version`   

To see the response from the server, the program `mosquitto_sub` must be running in a second console window:   
`mosquitto_sub -h 10.1.1.1 -t "#" -v`   
If this is the case, you will receive the following output:   
```   
modul1/get version
modul1/ret/version {"version":"2024-03-21 ESP32_03_smqtt_V1"}
```   

### Further Get commands   
* Query the help   
   `mosquitto_pub -h 10.1.1.1 -t modul1/get -m ?`   
   `mosquitto_pub -h 10.1.1.1 -t modul1/get -m help`   
* Query the IP address   
   `mosquitto_pub -h 10.1.1.1 -t modul1/get -m ip`   
* Query the base topic   
   `mosquitto_pub -h 10.1.1.1 -t modul1/get -m topicbase`   
* Query the EEPROM content   
   `mosquitto_pub -h 10.1.1.1 -t modul1/get -m eeprom`   

## MQTT set basic commands   
Set commands are used to set values or to control the hardware. They consist of the base topic and an appended `/set/xxx`, where `xxx` stands for the value to be changed. The value to which `xxx` is to be changed is specified in the message. In response, you receive a message whose topic is made up of the base topic and `/ret`.   

### Setting the base topic
Setting a new base topic immediately changes the value of the base topic. Furthermore, the value is stored in the EEPROM of the ESP32 and is retrieved from there each time the device is switched on.   
Note 1: The base topic only contains lower case letters!   
Note 2: If the basic topic is changed (and thus saved in the EEPROM), it remains changed even if the ESP32 is reprogrammed!   
When the ESP is started, a message is sent with the topic `info/start/mqtt`, which contains the base topic used:   
`info/start/mqtt {"topicbase":"modul1","IP":"10.1.1.185"}`   

__*Example*__: Setting the base topic to `Module1xxx`   
`mosquitto_pub -h 10.1.1.1 -t modul1/set/topicbase -m Modul1xxx`   
Response:   
`modul1a/ret/topicbase {"topicbase": "modul1xxx"}`   
Check by checking the EEPROM content:   
`mosquitto_pub -h 10.1.1.1 -t modul1xxx/get -m eeprom`   
Response:   
`modul1xxx/ret/eeprom {"topicbase": "modul1xxx", "mydata":"(no data)"}`   

### Deleting the EEPROM content
The EEPROM content is deleted using the command   
`mosquitto_pub -h 10.1.1.1 -t modul1/set/eeprom0 -m ?`   
where the value 1, 2 or 3 is set as the message, depending on what is to be deleted:   
1 = base topic, 2 = myData, 3 = everything   

[To the top of the page](#up)
