//_____D1_class_PCF8574.h________________________khartinger_____
// D1 mini class for i2c remote 8-Bit I/O Expander PCF8574.
// Default i2c address is 0x20 (other options 0x21...0x27).
// Created by Karl Hartinger, April 06, 2023
// Updates
//
// Released into the public domain.
#include "Arduino.h"              // bool, ...
#include <Wire.h>                 // i2c bus

//#ifndef D1_CLASS_PCF8574_H
//#define D1_CLASS_PCF8574_H
#define PCF8574_ADDR          0x20 // 7-bit address
#define PCF8574_STARTVALUE    0xFF // start byte
//-----status---------------------------------------------------
#define PCF8574_OK            0   // success
#define PCF8574_ERR_TOO_LONG  1   // i2c data too long to fit in transmit buffer 
#define PCF8574_ERR_NACK_ADDR 2   // i2c NACK transmit address
#define PCF8574_ERR_NACK_DATA 3   // i2c NACK transmit data
#define PCF8574_ERR_OTHER     4   // i2c other error
#define PCF8574_ERR_NUM_BYTES 5   // i2c wrong number of bytes
#define PCF8574_ERR_NO_BYTE   6   // i2c no byte received
#define PCF8574_ERR_BIT_NUM   7   // wrong bit number (not 0..7)
#define PCF8574_ERR_BIT_VAL   8   // wrong bit value (not 0|1)

// #define PCF8574_ERR_START     9   // first status after setup

class PCF8574 {
  //-----properties----------------------------------------------
 protected:
  int    i2cAddress;              // i2c address
  int    status;                  // state of i/o action
  bool   invertOutput;            // 
  byte   ioByte;                  // all 8 i/o ports
 //------constructor & co---------------------------------------
 public:
  PCF8574();                       // default constructor
  PCF8574(int i2c_address);        // constructor with i2c address
  PCF8574(int i2c_address, byte startvalue); // i2c address + start value
 protected:
  void setup();                   // init properties
  bool checkAddress(int i2c_address); // i2c address 0x20-0x27
 //------setter methods-----------------------------------------
 public:
  bool   setAddress(int i2c_address);
  bool   setBit(int bitnumber, int bitvalue);
  bool   setByte(byte ioByte_);
  void   setInvertOutput(bool doInvert);
 //------getter methods-----------------------------------------
  int    getAddress();            // return i2c address
  int    getStatus();             // status as int (0=OK)
  String getsStatus();            // status as text
  int    getByte();               // get pcf8574 inputs
  int    getBit(int bitnumber);
  bool   getInvertOutput();
 //------working methods----------------------------------------
  bool   begin();                 // startI2C=true
  bool   begin(bool startI2C);    // checkID, reset, set regs
 //------helper methods: i2c-access-----------------------------
 protected:
  bool   writeIoByte();           // write all 8 io pins
  bool   readIoByte();            // read all 8 io pins

};
//#endif
