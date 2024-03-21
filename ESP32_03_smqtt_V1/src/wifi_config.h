//_____wifi_config.h_____________________________khartinger_____
// Configure WiFi for ESP32_03_smqtt_v1.cpp
//
// Created by Karl Hartinger, March 21, 2024
// Changes:
// 2024-03-21 New
// Released into the public domain.

#ifndef WIFI_CONFIG_H
 #define WIFI_CONFIG_H
 #include <Arduino.h>                       // String, int32_t

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
#endif